const speedChartOption = {
    animationDuration: 1000,
    animationDurationUpdate: 80,
    animationEasing: 'quadraticOut',
    series: [
        {
            type: 'gauge',
            progress: {
                show: true,
                width: 4,
            },
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
            max: 120,
            splitNumber: 6,
            title: {
                show: false,
            },
            detail: {
                valueAnimation: true,
                fontSize: 24,
                offsetCenter: [0, '70%'],
            },
            data: [
                {
                    value: 0,
                },
            ],
        },
    ],
};

const angleChartOption = {
    animationDuration: 1000,
    animationDurationUpdate: 80,
    animationEasing: 'quadraticOut',
    series: [
        {
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
            splitNumber: 6,
            title: {
                show: false,
            },
            detail: {
                valueAnimation: true,
                fontSize: 24,
                offsetCenter: [0, '70%'],
            },
            data: [
                {
                    value: 90,
                },
            ],
        },
    ],
};

export {
    angleChartOption,
    speedChartOption
}