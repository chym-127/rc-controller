<template>
  <div class="slider-box" :id="uuid">
    <div class="slider-bar" :style="{ backgroundColor: unActiveColor }">
      <div class="slider"></div>
      <div class="run-bar" :style="{ backgroundColor: activeColor }"></div>
    </div>
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
    unActiveColor: {
      type: String,
      default: '#ebedf0',
    },
    activeColor: {
      type: String,
      default: 'rgb(25, 137, 250)',
    },
    backDefaultPos: {
      type: Boolean,
      default: false,
    },
  },
  data() {
    return {
      uuid: uuidv4(),
      el: null,
      runBarEl: null,
      pos: 0,
      current: 0,
      start: 0,
      move: 0,
      direction: 0,
      offset: 100,
    };
  },
  mounted() {
    this.init();
    window.requestAnimationFrame(this.animation);
  },
  methods: {
    animation() {
      this.el.style.left = this.current + '%';
      this.runBarEl.style.width = this.current + '%';
      this.$emit('onMove', this.current);
      window.requestAnimationFrame(this.animation);
    },
    init() {
      this.pos = this.defaultPos;
      this.current = this.defaultPos;

      this.el = document.getElementById(this.uuid).getElementsByClassName('slider')[0];
      this.runBarEl = document.getElementById(this.uuid).getElementsByClassName('run-bar')[0];
      this.el.style.left = this.current + '%'; 
      this.runBarEl.style.width = this.current + '%';
      const { maxY, minY } = this.getTouchRange();
      const parentNodeW = this.el.parentNode.clientWidth;

      this.el.addEventListener('touchstart', (event) => {
        if (event.targetTouches && event.targetTouches.length) {
          for (let index = 0; index < event.targetTouches.length; index++) {
            const touch = event.targetTouches[index];
            if (touch.clientY > minY && touch.clientY < maxY) {
              this.start = touch.clientX;
            }
          }
        }
      });
      this.el.addEventListener('touchmove', (event) => {
        if (event.targetTouches && event.targetTouches.length) {
          for (let index = 0; index < event.targetTouches.length; index++) {
            const touch = event.targetTouches[index];
            if (touch.clientY > minY && touch.clientY < maxY) {
              this.move = Math.abs(touch.clientX - this.start).toFixed(0);
              if (touch.clientX - this.start > 0) {
                this.direction = 1;
              }
              if (touch.clientX - this.start < 0) {
                this.direction = -1;
              }
              this.current = (Number(this.move) / parentNodeW) * 100 * this.direction + this.pos;
              if (this.current <= 0) {
                this.current = 0;
              }
              if (this.current >= 100) {
                this.current = 100;
              }
            }
          }
        }
      });
      this.el.addEventListener('touchend', (event) => {
        if (this.backDefaultPos) {
          this.pos = this.defaultPos;
          this.current = this.defaultPos;
          this.el.style.left = this.current + '%';
          this.$emit('onBack', this.pos);
        }
      });
    },

    getTouchRange() {
      let rect = this.el.getBoundingClientRect();
      let minY = parseInt(rect.top + window.scrollY - this.offset);
      let maxY = parseInt(rect.top + window.scrollY + this.offset + 30);
      return { minY, maxY };
    },
  },
};
</script>

<style scoped>
.slider-box {
  height: 30px;
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
}
.slider-bar {
  width: 100%;
  height: 2px;
  position: relative;
  border-radius: 12px;
}
.run-bar {
  width: 0%;
  height: 2px;
  transition: width 0.15s;
  border-radius: 12px;
}
.slider {
  position: absolute;
  background-color: #fff;
  border-radius: 10px;
  width: 35px;
  height: 35px;
  transition: left 0.15s;
  top: 50%;
  left: 50%;
  transform: translateX(-50%) translateY(-50%);
  box-shadow: 0 0.02667rem 0.05333rem rgba(0, 0, 0, 0.5);
}
</style>
