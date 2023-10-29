const speedChartOption = {
    animationDuration: 1000,
    animationDurationUpdate: 300,
    animationEasing: 'quadraticOut',
    series: [
        {
            type: 'gauge',
            startAngle: 180,
            endAngle: 0,
            min: 0,
            max: 100,
            splitNumber: 5,
            itemStyle: {
                color: '#58D9F9',
                shadowColor: 'rgba(0,138,255,0.45)',
                shadowBlur: 10,
                shadowOffsetX: 2,
                shadowOffsetY: 2
            },
            progress: {
                show: true,
                roundCap: true,
                width: 8
            },
            pointer: {
                icon: 'path://M2090.36389,615.30999 L2090.36389,615.30999 C2091.48372,615.30999 2092.40383,616.194028 2092.44859,617.312956 L2096.90698,728.755929 C2097.05155,732.369577 2094.2393,735.416212 2090.62566,735.56078 C2090.53845,735.564269 2090.45117,735.566014 2090.36389,735.566014 L2090.36389,735.566014 C2086.74736,735.566014 2083.81557,732.63423 2083.81557,729.017692 C2083.81557,728.930412 2083.81732,728.84314 2083.82081,728.755929 L2088.2792,617.312956 C2088.32396,616.194028 2089.24407,615.30999 2090.36389,615.30999 Z',
                length: '60%',
                width: 8,
                offsetCenter: [0, '5%']
            },
            axisLine: {
                roundCap: true,
                lineStyle: {
                    width: 8
                }
            },
            axisTick: {
                splitNumber: 2,
                lineStyle: {
                    width: 2,
                    color: '#999'
                }
            },
            splitLine: {
                length: 10,
                lineStyle: {
                    width: 3,
                    color: '#999'
                }
            },
            axisLabel: {
                distance: 20,
                color: '#999',
                fontSize: 12
            },
            title: {
                show: false
            },
            detail: {
                backgroundColor: '#fff',
                borderColor: '#999',
                borderWidth: 2,
                width: '100%',
                lineHeight: 20,
                height: 20,
                borderRadius: 8,
                offsetCenter: [0, '35%'],
                valueAnimation: true,
                formatter: function (value) {
                    return '{value|' + value.toFixed(0) * 15 + '}{unit|r/min}';
                },
                rich: {
                    value: {
                        fontSize: 16,
                        fontWeight: 'bolder',
                        color: '#777'
                    },
                    unit: {
                        fontSize: 16,
                        color: '#999',
                        padding: [0, 0, 0, 10]
                    }
                }
            },
            data: [
                {
                    value: 0
                }
            ]
        }
    ]
};

const angleChartOption = {
    animationDuration: 1000,
    animationDurationUpdate: 300,
    animationEasing: 'quadraticOut',
    series: [
        {
            startAngle: 180,
            endAngle: 0,
            type: 'gauge',
            axisLine: {
                lineStyle: {
                    width: 4,
                },
            },
            axisTick: {
                show: false,
            },
            splitLine: {
                length: 4,
                lineStyle: {
                    width: 2,
                    color: '#999',
                },
            },
            axisLabel: {
                distance: 14,
                color: '#999',
                fontSize: 14,
            },
            anchor: {
                show: true,
                showAbove: true,
                size: 12,
                itemStyle: {
                    borderWidth: 8,
                },
            },
            min: 0,
            max: 180,
            splitNumber: 3,
            title: {
                show: false,
            },
            detail: {
                valueAnimation: true,
                fontSize: 24,
                offsetCenter: [0, '70%'],
                formatter: function (value) {
                    return value === 30 ? '直行' : value > 30 ? '右' : '左';
                },
            },
            data: [
                {
                    value: 90,
                },
            ],
        },
    ],
};

const formatterSec = (seconds, format = 'HHMMSS') => {
    let hasHour = format.indexOf('HH') !== -1;
    let hasMinutes = format.indexOf('MM') !== -1;
    let hasSecond = format.indexOf('SS') !== -1;
    let temp = '';
    let sec = seconds <= 0 ? 0 : seconds.toFixed(0);
    let h, m, s, unit = 0;

    unit = 60 * 60;
    if (hasHour) {
        h = Math.trunc((sec / unit));
        if (h) {
            sec = sec % unit;
        }
    }

    unit = 60;
    if (hasMinutes) {
        m = Math.trunc((sec / unit));
        if (m) {
            sec = sec % unit;
        }
    } else {
        sec > 0 && h++;
    }

    if (hasSecond) {
        s = sec;
    } else {
        sec > 0 && hasMinutes && m++;
    }

    if (m >= 60) {
        m = 0;
        h++;
    }
    if (hasHour) {
        if (h < 10) {
            h = "0" + h
        }
        temp += `${h}:`;
    }

    if (hasMinutes) {
        if (m < 10) {
            m = "0" + m
        }
        temp += `${m}:`;
    }

    if (hasSecond) {
        if (s < 10) {
            s = "0" + s
        }
        temp += `${s}`;
    }

    return temp;
};


export {
    angleChartOption,
    speedChartOption,
    formatterSec
}