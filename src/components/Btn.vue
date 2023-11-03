<template>
  <div class="btn noselect" :id="uuid">
    <slot></slot>
  </div>
</template>

<script>
import { v4 as uuidv4 } from 'uuid';

export default {
  props: {
    defaultPos: {
      type: Number,
      default: 0,
    },
    backDefaultPos: {
      type: Boolean,
      default: true,
    },
    step: {
      type: Number,
      default: 1,
    },
    max: {
      type: Number,
      default: 100,
    },
    min: {
      type: Number,
      default: 0,
    },
    acceleration: {
      type: Number,
      default: 0,
    },
  },
  data() {
    return {
      uuid: uuidv4(),
      current: 0,
      preTime: 0,
      nowTime: 0,
      mouseDown: false,
      el: null,
    };
  },
  mounted() {
    this.current = this.defaultPos;
    this.init();
    window.requestAnimationFrame(this.animation);
  },
  methods: {
    animation(time) {
      if (this.mouseDown) {
        let diff = +new Date() - this.preTime;
        this.current += parseInt(this.step + (diff / 1000) * this.acceleration);
        if (this.current <= this.min) {
          this.current = this.min;
        }
        if (this.current >= this.max) {
          this.current = this.max;
        }
        this.$emit('onChange', { val: this.current, flag: false });
      }
      window.requestAnimationFrame(this.animation);
    },
    init() {
      this.el = document.getElementById(this.uuid);
      this.el.addEventListener('touchstart', (event) => {
        try {
          window.navigator.vibrate(100);
        } catch (error) {}
        this.mouseDown = true;
        this.preTime = +new Date();
      });

      this.el.addEventListener('touchend', (event) => {
        this.mouseDown = false;
        if (this.backDefaultPos) {
          this.current = this.defaultPos;
          this.$emit('onChange', { val: this.current, flag: true });
        }
      });
    },
  },
};
</script>

<style scoped>
.btn {
  border-radius: 10px;
  padding: 4px 12px;
  min-height: 80px;
  width: 80px;
  display: flex;
  justify-content: center;
  align-items: center;
  box-shadow: 0 0.02667rem 0.05333rem rgba(0, 0, 0, 0.5);
}

.noselect {
  -webkit-touch-callout: none;
  /* iOS Safari */
  -webkit-user-select: none;
  /* Safari */
  -khtml-user-select: none;
  /* Konqueror HTML */
  -moz-user-select: none;
  /* Old versions of Firefox */
  -ms-user-select: none;
  /* Internet Explorer/Edge */
  user-select: none;
  /* Non-prefixed version, currently
                                  supported by Chrome, Edge, Opera and Firefox */
}
</style>
