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
    formatterSec
}