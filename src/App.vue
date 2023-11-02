<script setup></script>

<template>
  <div class="w-screen flex flex-col" style="overflow: auto; height: 100dvh">
    <div class="flex-1">
      <div class="center flex flex-col">
        <div class="h-36 flex flex-row">
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

          <!-- <div class="flex-1">
            <span class="font-14-500 c-666">指令:&nbsp;&nbsp;</span>
            <span class="font-14-500" :style="{ color: fixedSpeed ? '#07c160' : '#969799' }">
              {{ fixedSpeed ? '开' : '关' }}
            </span>
          </div> -->

          <div class="flex-1">
            <span class="font-14-500 c-666">时长:&nbsp;&nbsp;</span>
            <span class="font-14-500" :style="{ color: durationEn ? '#07c160' : '#969799' }">
              {{ formatterSec(duration) }}
            </span>
          </div>
        </div>
        <div class="flex-1 flex flex-row mb-12 justify-between">
          <div class="flex-1 py-32">
            <div class="flex flex-row items-center mb-12">
              <p class="font-14-500 c-666 mr-12 labelW">最小启动速度:</p>
              <van-stepper :min="0" :max="100" v-model="config.minSpeed" step="2" />
            </div>
            <div class="flex flex-row items-center mb-12">
              <p class="font-14-500 c-666 mr-12 labelW">后退恒定速度开关:</p>
              <van-switch v-model="config.keepBackSpeed" />
            </div>
            <div class="flex flex-row items-center mb-12">
              <p class="font-14-500 c-666 mr-12 labelW">后退时恒定速度:</p>
              <van-stepper :min="0" :max="100" v-model="config.backSpeed" step="2" />
            </div>
          </div>
          <div class="road">
            <div class="car">
              <div class="wheel" style="top: 0; left: 0" :style="{ transform: `rotate(${wheelDeg}deg)` }"></div>
              <div class="line" style="top: 15px; left: 12px; right: 12px"></div>
              <div class="wheel" style="top: 0; right: 0" :style="{ transform: `rotate(${wheelDeg}deg)` }"></div>

              <div class="line w-2" style="top: 15px; left: 50%; bottom: 15px"></div>

              <div class="wheel" style="bottom: 0; left: 0"></div>
              <div class="line" style="bottom: 15px; left: 10px; right: 10px"></div>
              <div class="wheel" style="bottom: 0; right: 0"></div>
            </div>
            <div class="scroll-element" id="primary"></div>
            <div class="scroll-element" id="secondary"></div>
          </div>
          <div class="flex-1 py-32 pl-24">
            <div class="flex flex-row items-center mb-12">
              <p class="font-14-500 c-666 mr-12 labelW">转向速度:</p>
              <van-stepper :min="0" :max="30" v-model="config.trunStep" @change="trunStepChange" step="1" />
            </div>
            <div class="flex flex-row items-center mb-12">
              <p class="font-14-500 c-666 mr-12 labelW">转向范围:</p>
              <van-stepper :min="30" :max="120" v-model="config.trunRange" step="1" />
            </div>
          </div>
        </div>
        <div class="flex flex-row justify-between items-center" style="height: max-content">
          <div class="flex flex-row justify-center">
            <Btn :default-pos="20" @onChange="runningChange" :min="config.minSpeed" :max="100" :step="0.5">
              <span class="font-16-500 c-333">前进</span>
            </Btn>
            <div class="w-20"></div>
            <Btn :default-pos="20" @onChange="backChange" :min="20" :max="100" :step="0.5">
              <span class="font-16-500 c-333">倒车</span>
            </Btn>
          </div>

          <div class="flex flex-row justify-center">
            <van-button
              icon="pause-circle-o"
              :disabled="wsState != 2 || state == 2"
              plain
              type="danger"
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

          <div class="flex flex-row justify-center">
            <Btn :default-pos="50" @onChange="anglesChange" :step="config.trunStep" :min="50" :max="100">
              <span class="font-16-500 c-333">左</span>
            </Btn>
            <div class="w-20"></div>
            <Btn :default-pos="50" @onChange="anglesChange" :step="-config.trunStep" :min="0" :max="50">
              <span class="font-16-500 c-333">右</span>
            </Btn>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import Slider from './components/Slider.vue';
import Btn from './components/Btn.vue';
import { formatterSec } from './config';
export default {
  components: { Slider, Btn },
  data() {
    return {
      rotate: 0,
      formatterSec: formatterSec,
      duration: 0,
      durationEn: false,
      ip: '192.168.92.7',
      checked: false,
      angles: 50,
      config: {
        minSpeed: 20,
        trunRange: 60,
        keepBackSpeed: false,
        backSpeed: 20,
        trunStep: 1,
      },
      preAngles: -1,
      wheelDeg: 0,
      running: 0,
      preRunning: -1,
      offset: 52,
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
      setup: false,
      fixedSpeed: false,
    };
  },
  created() {
    let configStore = localStorage.getItem('CONFIG') ? JSON.parse(localStorage.getItem('CONFIG')) : null;
    if (configStore) {
      Object.assign(this.config, configStore);
    }
    //ceshi
    this.onOpen();
    this.initWebSocket();

    setInterval(() => {
      localStorage.setItem('CONFIG', JSON.stringify(this.config));
    }, 2000);
  },
  mounted() {
    setTimeout(() => {
      window.requestAnimationFrame(this.animation);
    }, 1500);
    setInterval(() => {
      if (this.durationEn) {
        this.duration += 1;
      }
    }, 1000);
  },
  methods: {
    anglesChange(data) {
      let val = data.val;
      if (val) {
        this.angles = val;
      }
    },
    runningChange(data) {
      let val = data.val;
      let flag = data.flag;

      this.gear = 'FORWARD';
      if (!flag && val) {
        this.running = val;
      }
      if (flag) {
        this.running = 0;
      }
    },
    backChange(data) {
      let val = data.val;
      let flag = data.flag;
      this.gear = 'BACKWARD';
      if (!flag && val) {
        if (this.config.keepBackSpeed) {
          this.running = this.config.backSpeed;
        } else {
          this.running = val;
        }
      }
      if (flag) {
        this.running = 0;
      }
    },
    runningSend(val) {
      let v = ((val / 100) * 255).toFixed(0);
      let speed = parseInt(v);
      if (speed) {
        this.sendMsg(this.gear, speed);
      } else {
        this.sendMsg('STOP', 0);
      }
    },
    anglesSend(val) {
      if (!val) {
        return;
      }
      let deg = 90;
      let diff = parseInt((Math.abs(50 - val) / (100 / 180)).toFixed(0));
      if (val > 50) {
        deg += parseInt(diff * (this.config.trunRange / 180));
      }
      if (val < 50) {
        deg -= parseInt(diff * (this.config.trunRange / 180));
      }
      this.sendMsg('TURN', deg);
    },
    animation() {
      if (this.wsState === 2) {
        if (this.preRunning !== this.running) {
          this.runningSend(this.running);
          this.preRunning = this.running;
          let cssSpeed = parseInt((3 - (this.running / 100) * 3).toFixed(2));
          if (cssSpeed <= 0.2) {
            cssSpeed = 0.2;
          }
          if (this.running === 0) {
            cssSpeed = 0;
          }

          document.getElementById('primary').style.animationDuration = `${cssSpeed}s`;
          document.getElementById('secondary').style.animationDuration = `${cssSpeed}s`;
        }
        if (this.preAngles !== this.angles) {
          this.anglesSend(this.angles);
          this.wheelDeg = 0;
          let diff = Math.abs(this.angles - 50) * 0.6;
          if (this.angles > 50) {
            this.wheelDeg = -diff;
          }
          if (this.angles < 50) {
            this.wheelDeg = diff;
          }
          this.preAngles = this.angles;
        }
      }
      window.requestAnimationFrame(this.animation);
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
    trunStepChange() {
      this.sendMsg('SET_SERVO_STEP', this.config.trunStep);
    },
    toggleGear() {
      this.gear = this.gear === 'FORWARD' ? 'BACKWARD' : 'FORWARD';
    },
    toggleFixedSpeed() {
      this.fixedSpeed = !this.fixedSpeed;
    },
    initWebSocket() {
      const gateway = ` ws://${this.ip}/ws`;
      this.websocket = new WebSocket(gateway);
      this.websocket.onopen = this.onOpen;
      this.websocket.onclose = this.onClose;
      this.websocket.onmessage = this.onMessage;
    },
    onOpen() {
      this.trunStepChange();
      this.durationEn = true;
      this.wsState = 2;
      this.start();
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
      } catch (error) {}
    },
    reset() {
      this.running = 0;
      this.angles = 50;
      this.fixedSpeed = false;
    },
    onClose() {
      this.durationEn = false;
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
  width: 100dvh;
}

/* .center {
  width: 100dvh;
  border: 1px solid;
  padding: 32px;
  height: calc(100vw);
  transform-origin: center;
  transform: rotate(90deg) translateX(218px) translateY(218px);
} */

.center {
  width: 100dvh;
  border: 1px solid;
  position: absolute;
  top: 50%;
  left: -50dvh;
  padding: 24px;
  height: 100vw;
  transform-origin: center center;
  transform: translateY(-50%) translateX(50vw) rotate(90deg);
  /* transform: rotate(0deg) translateX(50%) translateY(-50%); */
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

.h-200 {
  height: 200px;
}

.w-200 {
  width: 200px;
}

.hold-box {
  /* border: 1px solid red; */
  position: absolute;
  width: 25px;
  height: 25px;
  top: 50%;
  left: 50%;
  transform: translateX(-50%) translateY(-50%);
}

.road {
  position: relative;
  border: 1px solid;
  width: 120px;
  height: 100%;
  display: flex;
  background-color: #f0f0f0;
  justify-content: center;
  overflow: hidden;
}

.car {
  width: 60%;
  height: 70%;
  margin-top: 30px;
  /* border: 1px solid; */
  position: relative;
  z-index: 2;
}

.wheel {
  position: absolute;
  border: 2px solid;
  width: 10px;
  border-radius: 50px;
  height: 30px;
  transition: all 0.1;
}

.line {
  position: absolute;
  border: 1px solid;
  border-radius: 20px;
}

.scroll-element {
  z-index: 1;
  width: 12px;
  height: 100%;
  position: absolute;
  left: 50%;
  /* top: 0%; */
  transform: translateX(-50%);
  background: linear-gradient(
    to bottom,
    #fff 8.33%,
    #e0e0e0 8.33% 24.99%,
    #fff 24.99% 41.66%,
    #e0e0e0 41.66% 58.32%,
    #fff 58.32% 74.98%,
    #e0e0e0 74.98% 91.64%,
    #fff 91.64%
  );
}

.labelW {
  width: 120px;
}

.scroll-element#primary {
  animation: primary 0s linear infinite;
}

.scroll-element#secondary {
  animation: secondary 0s linear infinite;
}

@keyframes primary {
  from {
    top: 0%;
  }

  to {
    top: 100%;
  }
}

@keyframes secondary {
  from {
    top: -100%;
  }

  to {
    top: 0%;
  }
}
</style>
