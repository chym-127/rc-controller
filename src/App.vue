<script setup></script>

<template>
  <div class="px-12 py-32 w-screen h-screen flex flex-col" style="overflow: hidden">
    <div class="left mb-24">
      <div>
        <div>
          <span class="font-24-500 c-999 label">电机</span>
        </div>
        <div class="clear-both"></div>
      </div>
      <div class="flex flex-row jusity-center items-center">
        <div class="font-24-500 c-999 mr-12 label" style="float: left">
          <span>减</span>
        </div>
        <van-slider
          class="flex-1 w-0"
          :disabled="wsState != 2"
          v-model="running"
          @drag-end="dragRunningEnd"
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

          <div class="flex-1">
            <span class="font-14-500 c-666">定速:&nbsp;&nbsp;</span>
            <span class="font-14-500" :style="{ color: fixedSpeed ? '#07c160' : '#969799' }">
              {{ fixedSpeed ? '开' : '关' }}
            </span>
          </div>
        </div>
        <div class="flex-1 flex flex-row mb-12 justify-between">
          <div id="chartDomSpeed" class="w-0 flex-1 h-full"></div>
          <div id="chartDomAngle" class="w-0 flex-1 h-full"></div>
        </div>
        <div class="flex flex-row justify-center" style="height: max-content">
          <van-button :disabled="wsState != 2" plain type="primary" class="w-90" @click="toggleFixedSpeed">
            <span>定速</span>
          </van-button>
          <div class="w-20"></div>
          <van-button :disabled="wsState != 2" plain type="primary" class="w-90" @click="toggleGear">
            <span>切换档位</span>
          </van-button>
          <div class="w-20"></div>
          <van-button
            icon="pause-circle-o"
            :disabled="wsState != 2 || state == 2"
            plain
            type="primary"
            class="w-90"
            @click="stop"
          >
            <span>停止</span>
          </van-button>
          <div class="w-20"></div>
          <van-button
            icon="replay"
            :disabled="wsState != 2 || state == 1"
            plain
            type="primary"
            class="w-90"
            @click="start"
          >
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
          :disabled="wsState != 2"
          class="flex-1 w-0"
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
          <span class="font-24-500 c-999 label">舵机</span>
        </div>
        <div class="clear-both"></div>
      </div>
    </div>
  </div>
</template>

<script>
import * as echarts from 'echarts';

import { speedChartOption, angleChartOption } from './config';
let speedChart = null;
let angleChart = null;

export default {
  data() {
    return {
      rotate: 0,
      ip: '192.168.133.100',
      checked: false,
      angles: 50,
      preAngles: -1,
      running: 0,
      preRunning: -1,
      offset: 100,
      gear: 'FORWARD', // FORWARD 前进 BACKWARD 后退 3 停止
      state: 1, //1 启动 2 停止
      wsState: 2, //1 连接中 2 已连接 3 断开连接
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
      setup: false,

      fixedSpeed: false,
    };
  },
  created() {
    this.initWebSocket();
  },
  mounted() {
    this.initChart();
    setTimeout(() => {
      window.requestAnimationFrame(this.animation);
    }, 1500);
  },
  methods: {
    runningSend(val) {
      let v = ((val / 100) * (255 - this.offset)).toFixed(0);
      let speed = parseInt(v) + this.offset;
      let chartData = parseInt(((v / (255 - this.offset)) * 100).toFixed(0));
      let pre = speedChartOption.series[0].data[0].value;
      speedChartOption.series[0].data = [
        {
          value: speed === this.offset ? 0 : chartData,
        },
      ];
      this.sendMsg(speed === this.offset ? 'STOP' : this.gear, speed);
    },
    anglesSend(val) {
      let deg = 90;
      let diff = parseInt((Math.abs(50 - val) / 0.55555555).toFixed(0));
      if (val > 50) {
        deg += diff;
      }
      if (val < 50) {
        deg -= diff;
      }
      angleChartOption.series[0].data = [
        {
          value: deg,
        },
      ];

      this.sendMsg('TURN', deg);
    },
    animation() {
      if (this.wsState === 2) {
        if (this.preRunning !== this.running) {
          this.runningSend(this.running);
          this.preRunning = this.running;
        }
        if (this.preAngles !== this.angles) {
          this.anglesSend(this.angles);
          this.preAngles = this.angles;
        }
      }
      window.requestAnimationFrame(this.animation);
    },
    initChart() {
      let chartDom = document.getElementById('chartDomSpeed');
      let chartDomAngle = document.getElementById('chartDomAngle');
      speedChart = echarts.init(chartDom);
      angleChart = echarts.init(chartDomAngle);

      speedChartOption && speedChart.setOption(speedChartOption);
      angleChartOption && angleChart.setOption(angleChartOption);

      setTimeout(() => {
        setInterval(() => {
          speedChartOption && speedChart.setOption(speedChartOption);
          angleChartOption && angleChart.setOption(angleChartOption);
        }, 500);
      }, 1500);
    },
    start() {
      this.state = 1;
      this.sendMsg('ON');
    },
    stop() {
      this.state = 2;
      this.reset();
      this.sendMsg('OFF');
    },
    toggleGear() {
      this.gear = this.gear === 'FORWARD' ? 'BACKWARD' : 'FORWARD';
    },
    toggleFixedSpeed() {
      this.fixedSpeed = !this.fixedSpeed;
      if (!this.fixedSpeed) {
        this.running = 0;
      }
    },
    dragAnglesEnd() {
      if (this.angles !== 50) {
        this.angles = 50;
      }
    },
    dragRunningEnd() {
      if (this.running !== 0 && !this.fixedSpeed) {
        this.running = 0;
      }
    },
    initWebSocket() {
      const gateway = ` ws://${this.ip}/ws`;
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
      try {
        this.websocket.send(JSON.stringify(data));
        console.log(data);
      } catch (error) {}
    },
    reset() {
      this.running = 0;
      this.angles = 50;
      this.fixedSpeed = false;
    },
    onClose() {
      this.wsState = 3;
      this.reset();
      setTimeout(() => {
        this.initWebSocket();
      }, 1000);
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

.line {
  transition: all 0.4s;
}

.gauge-box {
  width: 200px;
  height: 200px;
  border: 1px solid red;
}

.gauge-box .scale {
  width: 100%;
  height: 100%;
  border: 10px solid #5470c6;
  border-radius: 50%;
  position: relative;
}

.gauge-box .scale .mask {
  width: 120px;
  height: 35px;
  background-color: #fff;
  transform: rotate(0deg) translateX(30px) translateY(155px);
  /* border: 1px solid black; */
}
/* // 315 225 */
.gauge-box .scale .anchor {
  position: absolute;
  top: 88px;
  left: 20px;
  width: 70px;
  height: 4px;
  transform-origin: 100% 50%;
  transform: rotate(-45deg);
  transition: transform 0.3s;
  background-color: black;
}
</style>
