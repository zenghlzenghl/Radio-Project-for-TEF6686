# 📻 TEF6686 收音控制程序 - STC8H单片机适配版本

> **目标芯片**: NXP TEF6686  
> **适配平台**: STC8H系列单片机 (8051内核)  
> **编译环境**: Keil C51  
> **创建日期**: 2026-10-20  
> **版本**: V1.0 (枚举类型优化版)

---

## 📁 项目文件结构

```
STC8H_TEF6686_Radio/
│
├── 🔧 核心驱动层/
│   ├── tef6686.h / tef6686.c      # TEF6686芯片驱动（寄存器配置+基本操作）
│   └── i2c_stc8h.h / i2c_stc8h.c  # STC8H软件模拟I2C驱动
│
├── 🎮 控制层/
│   └── radio_ctrl.h / radio_ctrl.c # 收音机控制逻辑（简洁API接口）
│
├── ⌨️ 按键驱动模块（非阻塞）/
│   ├── key.h / key.c               # 核心按键驱动（状态机+消抖+枚举事件）
│   └── key_config.h / key_config.c # 按键配置与回调函数
│
├── 💻 示例程序/
│   ├── main.c                      # 基础示例
│   ├── main_example.c              # 完整功能示例（阻塞式，简单）
│   ├── main_nonblocking.c          # 非阻塞示例（推荐⭐）
│   └── main_enum_example.c         # 枚举最佳实践示例（新增🆕）
│
└── 📚 文档/
    ├── README.md                    # 本文件（主文档）
    ├── KEY_MODULE_README.txt        # 按键模块详细说明书
    └── ENUM_UPDATE_LOG.txt          # 枚举类型更新日志
```

---

## 🔌 硬件连接

### TEF6686 与 STC8H 的 I2C 连接

| TEF6686引脚 | STC8H引脚 | 功能说明 |
|------------|----------|---------|
| SDA | P1.0 | I2C数据线 |
| SCL | P1.1 | I2C时钟线 |
| VCC | 3.3V | 电源（注意电平匹配！）|
| GND | GND | 地 |

> ⚠️ **重要**: TEF6686工作电压为**3.3V**，STC8H为5V系统时需要电平转换！

### 按键连接（可选，低电平有效）

| GPIO引脚 | 按键功能 | 编号 |
|---------|---------|------|
| P3.0 | 频率 + | `KEY_FREQ_UP` |
| P3.1 | 频率 - | `KEY_FREQ_DOWN` |
| P3.2 | Seek Up（搜索↑）| `KEY_SEEK_UP` |
| P3.3 | Seek Down（搜索↓）| `KEY_SEEK_DOWN` |
| P3.4 | 静音切换 | `KEY_MUTE` |
| P3.5 | 频段切换(FM/AM) | `KEY_BAND_SWITCH` |
| P3.6 | 音量 + | `KEY_VOLUME_UP` |
| P3.7 | 音量 - | `KEY_VOLUME_DOWN` |

> 💡 **提示**: 每个按键一端接GPIO，另一端接GND。使用STC8H内部上拉电阻。

### LED指示灯（可选）

| GPIO引脚 | 功能 | 说明 |
|---------|------|------|
| P1.2 | FM指示灯 | FM频段时常亮 |
| P1.3 | AM指示灯 | AM频段时常亮 |
| P1.4 | 静音指示灯 | 静音时常亮 |
| P1.5 | 立体声指示 | 接收立体声时常亮 |

---

## ⚡ 非阻塞按键模块特性

### ✨ 核心优势

- ✅ **完全不阻塞主程序** - 使用定时器中断 + 状态机架构
- ✅ **20ms软件消抖** - 稳定可靠，避免误触发
- ✅ **支持4种按键事件** - 短按、长按、按住、释放
- ✅ **连按功能** - 长按时可连续触发（如快速调频）
- ✅ **回调机制** - 事件驱动，松耦合设计
- ✅ **多按键支持** - 最多8个按键同时检测
- ✅ **极低CPU占用** - < 1%（8个按键，20ms扫描一次）
- ✅ **枚举类型** - 类型安全，IDE友好，调试方便 ⭐

### 🎮 按键事件类型（使用枚举 `KeyEvent_e`）

| 枚举值 | 数值 | 触发条件 | 典型应用场景 |
|-------|------|---------|-------------|
| `KEY_EVENT_NONE` | 0 | 无事件 | 初始状态/空闲 |
| `KEY_EVENT_SHORT` | 1 | 按下后**快速释放**（<1秒）| 单次操作（频率步进、静音切换）|
| `KEY_EVENT_LONG` | 2 | 按住**超过1秒** | 特殊功能（进入设置模式）|
| `KEY_EVENT_HOLD` | 3 | 长按期间**每500ms重复触发** | 连续操作（快速调频、连续调音量）|
| `KEY_EVENT_RELEASE` | 4 | **长按后松开** | 结束特殊模式 |

### 🔄 工作流程

```
定时器中断(10ms)
    ↓
Key_Config_Scan()  ←── 读取P3口GPIO状态
    ↓
Key_Scan()         ←── 执行状态机（立即返回，不阻塞！）
    ↓
回调函数执行业务逻辑
```

> 📖 **详细说明**: 请查看 [KEY_MODULE_README.txt](./KEY_MODULE_README.txt)

---

## 🛠️ 主要API函数

### 1️⃣ TEF6686 底层驱动 (`tef6686.h/c`)

#### 初始化与电源控制

```c
void TEF6686_Init(void);           // 完整初始化芯片
void TEF6686_PowerOn(void);        // 上电启动
void TEF6686_PowerOff(void);       // 关闭芯片
```

#### 频率调谐

```c
BOOL TEF6686_TuneToFreq(uint16 freq);  // 调谐到指定频率
uint16 TEF6686_GetFrequency(void);      // 获取当前频率
void TEF6686_SeekStart(uint8 direction);// 开始搜索(1=向上, 0=向下)
uint8 TEF6686_SeekStatus(void);        // 获取搜索状态(0=进行中, 1=找到台, 2=完成一圈)
```

#### 音量与静音

```c
void TEF6686_SetVolume(uint8 volume);  // 设置音量(0-255)
void TEF6686_Mute(uint8 mute);          // 静音控制(1=静音, 0=取消)
```

#### 信号质量检测

```c
uint8 TEF6686_GetLevel(void);    // 信号强度(dBμV)
uint8 TEF6686_GetUSN(void);      // 信噪比(%)
uint8 TEF6686_GetWAM(void);      // 多径干扰(%)
int16 TEF6686_GetOffset(void);   // 频率偏移(0.1kHz)
```

---

### 2️⃣ 控制层简化API (`radio_ctrl.h/c`)

> 💡 **推荐使用**: 这些函数封装了底层细节，更简洁易用。

```c
// 初始化
void RadioCtrl_Init(void);

// 频率控制
void RadioCtrl_SetFreq(uint16 freq);   // 设置频率
uint16 RadioCtrl_GetFreq(void);        // 获取当前频率
void RadioCtrl_FreqUp(void);           // 频率+
void RadioCtrl_FreqDown(void);         // 频率-

// 搜索控制
void RadioCtrl_SeekUp(void);           // 向上搜索
void RadioCtrl_SeekDown(void);         // 向下搜索

// 音量与静音
void RadioCtrl_MuteOn(void);           // 开启静音
void RadioCtrl_MuteOff(void);          // 关闭静音
void RadioCtrl_SetVolume(uint8 vol);   // 设置音量

// 频段切换
void RadioCtrl_SetBand(uint8 band);    // 设置频段(BAND_FM / BAND_AM)
uint8 RadioCtrl_GetBand(void);         // 获取当前频段

// 状态查询
uint8 RadioCtrl_GetLevel(void);        // 获取信号强度
```

---

### 3️⃣ 按键模块API (`key.h/c`)

#### 初始化与配置

```c
void Key_Init(void);
void Key_Config_Init(void);             // 初始化+注册所有回调（推荐）
void Key_RegisterCallback(uint8 key_num, key_callback_t callback);
```

#### 状态更新（在定时器中调用）

```c
void Key_Config_Scan(void);             // 读取GPIO+扫描（建议20ms调用一次）
void Key_SetPinState(uint8 key_num, uint8 state);  // 手动设置引脚状态
```

#### 事件查询

```c
KeyEvent_e Key_GetEvent(uint8 key_num);  // 获取事件（枚举类型）⭐
uint8 Key_IsPressed(uint8 key_num);      // 是否正在按下
void Key_ClearEvent(uint8 key_num);       // 清除事件
```

---

## 📻 频率范围与步进

### FM频段

| 参数 | 中国/欧洲 | 日本 | 美国 |
|-----|---------|------|------|
| **频率范围** | 87.5 - 108.0 MHz | 76 - 90 MHz | 87.5 - 108.0 MHz |
| **编程单位** | 10kHz | 10kHz | 10kHz |
| **最小值** | 8750 | 7600 | 8750 |
| **最大值** | 10800 | 9000 | 10800 |
| **步进** | 100kHz (10) | 50kHz (5) | 100kHz (10) |

### AM频段

| 参数 | 中国/欧洲 | 美国 |
|-----|---------|------|
| **频率范围** | 531 - 1620 kHz | 530 - 1710 kHz |
| **编程单位** | 1kHz | 1kHz |
| **最小值** | 531 | 530 |
| **最大值** | 1620 | 1710 |
| **步进** | 9kHz | 10kHz |

> 📝 **编程示例**: 调谐到98.5MHz → `TEF6686_TuneToFreq(9850)`

---

## 🚀 快速开始（5步骤）

### Step 1: 创建Keil工程

1. 打开 **Keil uVision**
2. 新建工程 → 选择芯片 **STC8Hx系列**（如STC8H8K64U）
3. 添加所有 `.c` 文件到工程

### Step 2: 配置头文件路径

确保包含STC8H的头文件路径（`stc8h.h`）

### Step 3: 修改硬件配置（如需要）

根据实际电路修改IO口定义：

```c
// i2c_stc8h.h 中修改I2C引脚
#define I2C_SCL  P1_1  // 可改为其他引脚
#define I2C_SDA  P1_0  // 可改为其他引脚

// key_config.c 中修改按键引脚
Key_SetPinState(KEY_FREQ_UP, P3_0);  // 可改用其他端口
```

### Step 4: 编译工程

按 **F7** 或点击 **Build** 按钮，确保无错误无警告

### Step 5: 下载运行

连接STC-ISP下载器，下载到STC8H单片机，复位运行！

---

## 💡 使用示例

### 示例1：非阻塞式主程序（推荐）⭐

查看 [main_nonblocking.c](./main_nonblocking.c)

```c
#include "stc8h.h"
#include "key_config.h"
#include "radio_ctrl.h"

void Timer0_ISR(void) interrupt 1
{
    // 重装定时器值(10ms)
    TH0 = (65536 - 10000) / 256;
    TL0 = (65536 - 10000) % 256;

    static uint8 scan_cnt = 0;
    scan_cnt++;
    if (scan_cnt >= 2) {           // 20ms扫描一次
        scan_cnt = 0;
        Key_Config_Scan();          // ⭐ 非阻塞！立即返回
    }
}

void main(void)
{
    GPIO_Init();            // 初始化GPIO
    Timer0_Init();          // 启动10ms定时器中断
    Key_Config_Init();      // 初始化按键并注册回调
    RadioCtrl_Init();       // 初始化收音机

    while (1) {
        // ✅ 主循环完全自由！可以同时处理多个任务
        Task_10ms();        // 10ms任务
        Task_100ms();       // 100ms任务（显示更新等）
        Task_1s();          // 1秒任务
    }
}
```

**特点**：
- ✅ 主循环**永不阻塞**
- ✅ 按键检测在**后台自动完成**
- ✅ 可轻松扩展多任务

---

### 示例2：枚举类型最佳实践 🆕

查看 [main_enum_example.c](./main_enum_example.c)

```c
#include "key_config.h"

void HandleKeyEvent(uint8 key_num, KeyEvent_e event)
{
    switch (key_num) {
        case KEY_FREQ_UP:
            switch (event) {
                case KEY_EVENT_SHORT:
                    printf("短按: 频率+\n");
                    RadioCtrl_FreqUp();
                    break;
                case KEY_EVENT_LONG:
                    printf("长按: 频率+\n");
                    break;
                case KEY_EVENT_HOLD:
                    printf("按住: 频率+ (连按)\n");
                    RadioCtrl_FreqUp();
                    break;
                case KEY_EVENT_RELEASE:
                    printf("释放\n");
                    break;
                default:
                    break;
            }
            break;

        case KEY_MUTE:
            if (event == KEY_EVENT_SHORT) {
                if (RadioParam.mute_status) {
                    RadioCtrl_MuteOff();
                } else {
                    RadioCtrl_MuteOn();
                }
            }
            break;

        // ... 其他按键处理
    }
}

void PollKeys(void)
{
    uint8 i;
    for (i = 0; i < KEY_NUM; i++) {
        KeyEvent_e event = Key_GetEvent(i);  // 返回枚举类型 ⭐

        if (event != KEY_EVENT_NONE) {
            HandleKeyEvent(i, event);
            Key_ClearEvent(i);
        }
    }
}
```

**特点**：
- ✅ 使用**枚举类型**，类型安全
- ✅ **Switch语句**，清晰易读
- ✅ **嵌套处理**，逻辑分明
- ✅ IDE**智能提示**，开发效率高

---

### 示例3：最简用法

查看 [main.c](./main.c)

```c
#include "tef6686.h"

void main(void)
{
    TEF6686_Init();
    TEF6686_SetVolume(70);

    while (1) {
        // 直接调用API（简单但会阻塞）
        TEF6686_TuneToFreq(9850);  // 调到98.5MHz
        Delay_ms(1000);
    }
}
```

**适用场景**：学习测试、简单应用

---

## ⚙️ 枚举类型详解 (`KeyEvent_e`) ⭐

### 定义位置

[**key.h**](./key.h#L15-L21)

```c
typedef enum {
    KEY_EVENT_NONE = 0,    // 无事件
    KEY_EVENT_SHORT,       // 短按 (1)
    KEY_EVENT_LONG,        // 长按 (2)
    KEY_EVENT_HOLD,        // 按住 (3)
    KEY_EVENT_RELEASE      // 释放 (4)
} KeyEvent_e;
```

### 为什么使用枚举？

| 对比项 | 宏定义 (`#define`) | 枚举 (`enum`) |
|-------|-------------------|---------------|
| **类型安全** | ❌ 可传入任意数字 | ✅ 编译器检查类型 |
| **调试体验** | 显示数字 `event=2` | 显示名称 `event=KEY_EVENT_LONG` |
| **IDE支持** | 无智能提示 | 自动补全枚举值 |
| **代码可读性** | `if (event == 2)` | `if (event == KEY_EVENT_LONG)` |
| **错误预防** | 容易写错数值 | 不可能拼写错误 |

### 在回调函数中使用

```c
// 回调函数原型（自动类型检查）
void MyCallback(uint8 key_num, KeyEvent_e event);

// 推荐写法：使用switch
void Key_VolumeUp_Callback(uint8 key_num, KeyEvent_e event)
{
    switch (event) {
        case KEY_EVENT_SHORT:     // 短按：音量+5
        case KEY_EVENT_HOLD:      // 按住：连续加音量
            current_volume += 5;
            RadioCtrl_SetVolume(current_volume);
            break;
        default:
            break;  // 忽略长按和释放
    }
}
```

> 📖 **完整教程**: 查看 [ENUM_UPDATE_LOG.txt](./ENUM_UPDATE_LOG.txt)

---

## ⚠️ 注意事项

### 硬件相关

1. **I2C速率**: 建议设置在 **100kHz以下**，确保通信稳定
2. **工作电压**: TEF6686为 **3.3V**，5V系统需电平转换
3. **上拉电阻**: 按键和I2C都需要上拉（可用STC8H内部上拉）

### 软件相关

4. **信号稳定时间**: 调谐后需等待 **30-50ms** 让信号稳定
5. **Seek异步操作**: `TEF6686_SeekStart()` 是异步的，需轮询 `TEF6686_SeekStatus()`
6. **音量范围**: 0-255，对应约 **-60dB 到 +10dB**
7. **初始化时序**: 上电后建议等待 **100ms** 再初始化

### 性能优化

8. **定时器频率**: 按键扫描建议 **20ms** 一次（平衡响应速度和CPU占用）
9. **避免浮点运算**: STC8H无硬件FPU，尽量用定点数

---

## 🐛 调试建议

### 1. 串口打印调试

```c
// 打印当前频率和信号强度
printf("Freq: %d.%d MHz  Level: %d dBuV\n",
       freq/10, freq%10, level);
```

### 2. I2C通信检查

- 用示波器观察SCL/SDA波形
- 检查起始/停止条件是否正确
- 确认应答信号(ACK)是否正常

### 3. TEF6686响应验证

```c
// 读取芯片ID（应该返回特定值）
uint8 id = ReadChipID();
printf("TEF6686 ID: 0x%02X\n", id);
```

### 4. 音频输出测试

- 用示波器/耳机检查音频输出
- 验证是否有音频信号输出
- 测试左右声道是否正常

---

## 📦 从原工程提取的内容

本程序从 **HC32F4A0 (ARM Cortex-M4)** 工程的 `IVI_Radio` 模块提取并适配：

### 提取的核心功能

#### 1️⃣ TEF6686寄存器初始化参数

| 类别 | 参数数量 | 包含内容 |
|-----|---------|---------|
| **FM初始化** | 28个寄存器 | 带宽、软静音、高切/低切滤波、立体声、去加重、RDS等 |
| **AM初始化** | 7个寄存器 | 带宽、低切滤波等 |
| **Audio/Appl** | 10个寄存器 | 音量、GPIO配置、I2S输出等 |

**来源文件**: [RadioDrv_TEF668X.c#L96-L200](原工程Source/IVI_Radio/RadioDrv_TEF668X.c)

#### 2️⃣ 质量检测算法

```c
#define QUALITY_DATA_VALID  32    // 32ms后数据有效
#define FM_USN              10    // 信噪比阈值: 10%
#define FM_WAM              20    // 多径干扰阈值: 20%
#define FM_OFFSET           100   // 频偏阈值: 10kHz
#define FM_BANDWIDTH        133   // 带宽: 133kHz
```

**用途**: 判断当前电台是否为"好台"，决定是否停止搜索

#### 3️⃣ 搜索算法

- **自动搜索(Seek)**: 向上/向下搜索下一个有效电台
- **台位判断标准**: 基于信号强度、信噪比、多径干扰综合判断
- **灵敏度阈值**: 可配置，适应不同环境

### 适配改动

| 项目 | 原工程 (HC32F4A0) | 本版本 (STC8H) |
|-----|------------------|--------------|
| **MCU内核** | ARM Cortex-M4 (32位) | 8051 (8位) |
| **编译器** | ARM GCC | Keil C51 |
| **数据类型** | int8u, int16u, BOOL | uint8, uint16, uint8 |
| **I2C驱动** | 硬件I2C + 模拟I2C | 纯软件模拟I2C |
| **RTOS依赖** | FreeRTOS任务调度 | 无RTOS，轮询方式 |
| **代码规模** | ~5000行 (含RDS/DAB) | ~1200行 (核心功能) |
| **内存占用** | 较大 | 优化适合STC8H (8KB RAM) |

### 原工程参考文件

```
Source/IVI_Radio/
├── RadioDrv_TEF668X.c/h    # TEF6686原始驱动 ⭐主要参考
├── RadioCtrl.c/h           # 控制逻辑
├── RadioCommon.h           # 公共定义
├── RadioApi.c/h            # API接口
└── RDS/                    # RDS功能（未移植）

Source/Bus/
├── i2c_bus.c/h             # I2C总线驱动 ⭐
└── i2c_Std_drive.c/h       # I2C标准驱动
```

---

## 📊 性能指标

| 项目 | 数值 | 说明 |
|-----|------|------|
| **CPU占用率** | < 1% | 8个按键，20ms扫描 |
| **RAM占用** | ~300字节 | 含按键+收音机状态 |
| **ROM占用** | ~4KB | 核心功能代码 |
| **按键响应延迟** | 20ms | 消抖时间 |
| **长按检测时间** | 1000ms | 可配置 |
| **连按间隔** | 200ms | 可配置 |
| **I2C速率** | ≤100kHz | 确保稳定性 |
| **支持按键数** | 8个 | 可扩展 |

---

## 🔧 时间参数配置

在 [**key.h**](./key.h) 中修改：

```c
#define KEY_DEBOUNCE_TIME    20   // 消抖时间(ms)，推荐10-30ms
#define KEY_LONG_TIME        1000 // 长按判定(ms)，推荐800-1500ms
#define KEY_REPEAT_TIME      200  // 连续短按间隔(ms)，推荐100-300ms
#define KEY_HOLD_TIME        500  // 按住事件间隔(ms)，推荐300-800ms
```

**调整建议**：
- 更灵敏 → 减小 `KEY_DEBOUNCE_TIME`
- 更稳定 → 增大 `KEY_DEBOUNCE_TIME`
- 更快连按 → 减小 `KEY_REPEAT_TIME`
- 更慢连按 → 增大 `KEY_REPEAT_TIME`

---

## 🎯 适用场景

✅ **汽车收音机** - 非阻塞设计保证实时性  
✅ **便携式音响** - 低功耗，资源占用小  
✅ **学习开发** - 代码清晰，注释完善  
✅ **产品原型** - 快速验证功能  
✅ **工业控制** - 稳定可靠的状态机  

---

## 📈 版本历史

| 版本 | 日期 | 更新内容 |
|-----|------|---------|
| **V1.0** | 2026-10-20 | 初始版本，从HC32F4A0工程提取并适配STC8H |
| **V1.1** | 2026-10-20 | 新增非阻塞按键模块（状态机+定时器）|
| **V1.2** | 2026-10-20 | 按键事件升级为枚举类型 `KeyEvent_e` ⭐ |

---

## 📄 许可证

本项目从开源汽车收音机工程提取适配，仅供学习和研究使用。

---

## 🙏 致谢

- **NXP Semiconductors** - TEF6686芯片及官方驱动
- **STC Microelectronics** - STC8H系列单片机
- **原工程团队** - HC32F4A0平台的基础代码

---

## 📞 技术支持

如有问题，请按以下顺序排查：

1. 📖 查看本文档和 [KEY_MODULE_README.txt](./KEY_MODULE_README.txt)
2. 🔍 检查硬件连接和电源电压
3. 🐛 使用串口打印调试信息
4. 📊 用示波器观察I2C波形
5. 💬 参考原工程代码对比差异

---

<div align="center">

**祝您开发顺利！** 🎉  

*如有改进建议，欢迎反馈！*

</div>