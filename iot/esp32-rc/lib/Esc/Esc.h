
#define DEFAULT_uS_MIN 10   // 最小脉宽 单位：us
#define DEFAULT_uS_MAX 2500 // 最大脉宽 单位：us
#define TIMER_10_BIT 10     // 10位分辨率
#define TIMER_12_BIT 12     // 12位分辨率
#define TIMER_15_BIT 15     // 15位分辨率
#define DEFAULT_FREQ 50     // 默认赫兹
#define MAX_FREQ 5000       // 最大赫兹

class Esc
{
private:
    void writeDuty(uint32_t duty);
    uint32_t usecToDuty(uint32_t usec); // 脉宽时间转占空比
    int minUs = DEFAULT_uS_MIN;         // 可写入的最小脉宽时间 单位：us
    int maxUs = DEFAULT_uS_MAX;         // 可写入的最大脉宽时间 单位：us
    int freq = DEFAULT_FREQ;            // 赫兹
    int pinNumber = 0;                  // 引脚
    int channel = 0;                    // 通道索引 0-15
    int timerWidth = TIMER_10_BIT;      // 分辨率位宽
    double ticks = 0;                   // 脉冲周期 单位：us

public:
    Esc();
    int resolution = 0;
    int resolutionMiddle = 0;
    int getTicks();
    void setUp(uint8_t channel);
    void setUp(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
    void attachPin(uint8_t pin);
    void writeMicroseconds(uint32_t usec);
    void write(uint32_t duty);
    void write(uint32_t val, uint32_t min, uint32_t max);
    void write(uint32_t val, uint32_t min, uint32_t max, uint32_t oMin, uint32_t oMax);
};
