<script setup></script>

<template>
  <div class="p-12 w-screen h-screen flex flex-col" style="overflow: hidden">
    <div class="left">
      <div>
        <div>
          <span class="font-24-500 c-999 label">油门</span>
        </div>
        <div class="clear-both"></div>
      </div>
      <div class="flex flex-row jusity-center items-center">
        <div class="font-24-500 c-999 mr-12 label" style="float: left">
          <span>加</span>
        </div>
        <van-slider
          class="flex-1 w-0"
          v-model="running"
          @drag-end="dragRunningEnd"
          inactive-color="#1989fa"
          active-color="#1989fa"
        />
        <div class="font-24-500 c-999 ml-12 label" style="float: right">
          <span>减</span>
        </div>
      </div>
    </div>
    <div class="center flex-1 w-0 py-32 rotate-90">
      <van-grid clickable class="">
        <van-grid-item text="启动" icon="play-circle-o" @click="start" />
        <van-grid-item text="停止" icon="pause-circle-o" @click="stop" />
        <van-grid-item text="重置" icon="replay" @click="reset" />
      </van-grid>
    </div>
    <div class="right">
      <div class="flex flex-row jusity-center items-center">
        <div class="font-24-500 c-999 mr-12 label" style="float: left">
          <span>左</span>
        </div>
        <van-slider
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
          <span class="font-24-500 c-999 label">转向</span>
        </div>
        <div class="clear-both"></div>
      </div>
    </div>
  </div>
</template>

<script>
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
      running: 50,
      angles: 50,
      setup: false,
    };
  },
  watch: {
    running(val) {
      $delay(() => {
        let cmd = 'STOP';
        let v = ((Math.abs(50 - val) / 50) * 255).toFixed(0);
        if (val > 50) {
          cmd = 'FORWARD';
        }
        if (val < 50) {
          cmd = 'BACKWARD';
        }

        this.sendMsg(cmd, v);
      });
    },
    angles(val) {
      $delay(() => {
        let deg = 90;
        let diff = Math.abs(50 - val) / 0.55555555;
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
  methods: {
    start() {
      this.sendMsg('ON');
    },
    stop() {
      this.sendMsg('OFF');
    },
    dragAnglesEnd() {
      if (this.angles !== 50) {
        this.angles = 50;
      }
    },
    dragRunningEnd() {
      if (this.running !== 50) {
        this.running = 50;
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
      console.log('onOpen');
      this.websocket.send(JSON.stringify(data));
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
      console.log('onClose');
    },
    onMessage() {
      console.log('onMessage');
    },
  },
};
</script>

<style scoped>
.label {
  writing-mode: vertical-rl;
  text-orientation: upright;
  transform: rotate(90deg);
  float: right;
}

.rotate-90 {
  transform: rotate(90deg);
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
</style>
