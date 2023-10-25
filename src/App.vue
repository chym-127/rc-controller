<script setup></script>

<template>
  <div class="px-12 py-32 w-screen h-screen flex flex-col" style="overflow: hidden">
    <div class="left mb-24">
      <div>
        <div>
          <span class="font-24-500 c-999 label">油门</span>
        </div>
        <div class="clear-both"></div>
      </div>
      <div class="flex flex-row jusity-center items-center">
        <div class="font-24-500 c-999 mr-12 label" style="float: left">
          <span>减</span>
        </div>
        <van-slider
          :disabled="wsState != 2"
          class="flex-1 w-0"
          v-model="running"
          @drag-end="dragRunningEnd"
          inactive-color="#1989fa"
          active-color="#1989fa"
        />
        <div class="font-24-500 c-999 ml-12 label" style="float: right">
          <span>加</span>
        </div>
      </div>
    </div>
    <div class="flex-1">
      <div class="center flex flex-col">
        <div class="h-60 flex flex-row">
          <div class="flex-1">
            <span class="font-14-500 c-666">状态:&nbsp;&nbsp;</span>
            <span class="font-14-500" :style="{ color: wsStateMapper[wsState].color }">
              {{ wsStateMapper[wsState].txt }}
            </span>
          </div>

          <div class="flex-1">
            <span class="font-14-500 c-666">档位:&nbsp;&nbsp;</span>
            <span class="font-14-500" :style="{ color: gearMapper[gear].color }">
              {{ gearMapper[gear].txt }}
            </span>
          </div>
        </div>
        <div class="flex-1 flex flex-row mb-12 justify-between">
          <div id="chartDomSpeed" class="w-0 flex-1 h-full"></div>
          <div class="w-0 flex-1"></div>
        </div>
        <div class="flex flex-row justify-center" style="height: max-content">
          <van-button :disabled="wsState != 2" plain type="primary" class="w-90" @click="toggleGear">
            <span>切换档位</span>
          </van-button>
          <div class="w-20"></div>
          <van-button icon="pause-circle-o" :disabled="wsState != 2 || state == 2" plain type="primary" class="w-90">
            <span>停止</span>
          </van-button>
          <div class="w-20"></div>
          <van-button icon="replay" :disabled="wsState != 2 || state == 1" plain type="primary" class="w-90">
            <span>启动</span>
          </van-button>
        </div>
      </div>
    </div>
    <div class="right mt-24">
      <div class="flex flex-row jusity-center items-center">
        <div class="font-24-500 c-999 mr-12 label" style="float: left">
          <span>左</span>
        </div>
        <van-slider
          class="flex-1 w-0"
          :disabled="wsState != 2"
          v-model="angles"
          @drag-end="dragAnglesEnd"
          inactive-color="#1989fa"
          active-color="#1989fa"
        />
        <div class="font-24-500 c-999 ml-12 label" style="float: right">
          <span>右</span>
        </div>
      </div>
      <div>
        <div>
          <span class="font-24-500 c-999 label">转向</span>
        </div>
        <div class="clear-both"></div>
      </div>
    </div>
  </div>
</template>

<script>
import * as echarts from 'echarts';
const $delay = (function () {
  let timer = 0;
  return function (callback, ms) {
    clearTimeout(timer);
    timer = setTimeout(callback, ms);
  };
})();
export default {
  data() {
    return {
      checked: false,
      running: 0,
      offset: 100,
      gear: 'FORWARD', // FORWARD 前进 BACKWARD 后退 3 停止
      state: 1, //1 启动 2 停止
      wsState: 1, //1 连接中 2 已连接 3 断开连接
      gearMapper: {
        FORWARD: {
          txt: '前进',
          color: '#07c160',
        },
        BACKWARD: {
          txt: '后退',
          color: '#FAAB0C',
        },
        STOP: {
          txt: '停止',
          color: '#969799',
        },
      },
      wsStateMapper: {
        1: {
          txt: '连接中',
          color: '#969799',
        },
        2: {
          txt: '已连接',
          color: '#07c160',
        },
        3: {
          txt: '断开连接',
          color: '#ee0a24',
        },
      },
      angles: 50,
      setup: false,
      speedChart: null,
      speedChartOption: null,
    };
  },
  watch: {
    running(val) {
      $delay(() => {
        let v = ((val / 100) * (255 - this.offset)).toFixed(0);
        let speed = parseInt(v) + this.offset;
        let chartData = parseInt(((speed / 255) * 100).toFixed(0));

        this.speedChartOption.series[0].data = [
          {
            value: speed === this.offset ? 0 : chartData,
          },
        ];
        this.sendMsg(speed === this.offset ? 'STOP' : this.gear, speed);
      });
    },
    angles(val) {
      $delay(() => {
        let deg = 90;
        let diff = parseInt((Math.abs(50 - val) / 0.55555555).toFixed(0));
        if (val > 50) {
          deg -= diff;
        }
        if (val < 50) {
          deg += diff;
        }
        this.sendMsg('TURN', deg);
      });
    },
  },
  created() {
    this.initWebSocket();
  },
  mounted() {
    this.initChart();
  },
  methods: {
    initChart() {
      let chartDom = document.getElementById('chartDomSpeed');
      this.speedChart = echarts.init(chartDom);
      this.speedChartOption = {
        series: [
          {
            type: 'gauge',
            progress: {
              show: true,
              width: 5,
            },
            axisLine: {
              lineStyle: {
                width: 5,
              },
            },
            axisTick: {
              show: false,
            },
            splitLine: {
              length: 12,
              lineStyle: {
                width: 2,
                color: '#999',
              },
            },
            axisLabel: {
              distance: 15,
              color: '#999',
              fontSize: 12,
            },
            anchor: {
              show: true,
              showAbove: true,
              size: 14,
              itemStyle: {
                borderWidth: 10,
              },
            },
            title: {
              show: false,
            },
            detail: {
              valueAnimation: true,
              fontSize: 20,
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

      this.speedChartOption && this.speedChart.setOption(this.speedChartOption);
      setTimeout(() => {
        setInterval(() => {
          this.speedChart.setOption(this.speedChartOption, true);
        }, 400);
      }, 100);
    },
    start() {
      this.sendMsg('ON');
    },
    stop() {
      this.sendMsg('OFF');
    },
    toggleGear() {
      this.gear = this.gear === 'FORWARD' ? 'BACKWARD' : 'FORWARD';
    },
    dragAnglesEnd() {
      if (this.angles !== 50) {
        this.angles = 50;
      }
    },
    dragRunningEnd() {
      if (this.running !== 0) {
        this.running = 0;
      }
    },
    initWebSocket() {
      const gateway = ` ws://192.168.2.157/ws`;
      console.log('Trying to open a WebSocket connection…');
      this.websocket = new WebSocket(gateway);
      this.websocket.onopen = this.onOpen;
      this.websocket.onclose = this.onClose;
      this.websocket.onmessage = this.onMessage;
    },
    onOpen() {
      this.wsState = 2;
    },
    sendMsg(cmd, val = '') {
      if (!cmd) {
        console.log('cmd is emtry');
        return;
      }
      let data = {
        COMMOND: cmd,
        VALUE: val,
      };
      console.log(data);
      this.websocket.send(JSON.stringify(data));
    },
    onClose() {
      this.wsState = 3;
    },
    onMessage() {
      console.log('onMessage');
    },
  },
};
</script>

<style scoped>
.w-center-box {
  border: 1px solid;
  width: calc(100vh - 280px);
}

.center {
  width: calc(100vh - 256px);
  height: calc(100vw - 24px);
  transform-origin: center;
  padding: 0 24px;
  transform: rotate(90deg) translateX(102px) translateY(100px);
}

.label {
  writing-mode: vertical-rl;
  text-orientation: upright;
  transform: rotate(90deg);
  float: right;
}

.rotate-90 {
  transform: rotate(90deg);
}

.rotate-90-center {
  transform: rotate(90deg) translateX(50%);
}

.rotate-180 {
  transform: rotate(270deg);
}

@media screen and (orientation: portrait) {
  /*要横屏显示的内容的外层盒子*/
  .box {
    /* 设置基准点为左上角 */
    transform-origin: top left;
    /* 以左上角为基准点，顺时针旋转90度 然后沿Y轴反方向平移100% */
    /* 为什么沿Y轴，因为旋转使坐标轴方向发生了变化 */
    transform: rotate(90deg) translateY(-100%);
    /*设置元素的宽度为 100%父元素的高度 并提高样式优先级 */
    width: 100vh !important;
    /*设置元素的宽度为 100%父元素的宽度  并提高样式优先级 */
    height: 100vw !important;
  }
}

.box {
  width: 100%;
  height: 100%;
  display: flex;
  border: 1px solid rebeccapurple;
}

.h-200 {
  height: 200px;
}

.w-200 {
  width: 200px;
}
</style>
