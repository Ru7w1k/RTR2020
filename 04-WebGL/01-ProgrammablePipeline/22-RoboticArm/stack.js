class Stack {
    constructor() {
        this.data = new Array(10);
        this.sp = -1;
    }

    push(d) {
        if (this.sp < 9) {
            this.sp++;
            var a = mat4.create();
            Object.assign(a, d);
            this.data[this.sp] = a;
            // console.log("stack push", this.sp, d, this.data);
        }
    }

    pop() {
        if (this.sp > -1) {
            this.sp--;
            // console.log("stack pop", this.sp, this.data[this.sp + 1]);
            return this.data[this.sp + 1];
        }
    }

    reset() {
        this.sp = -1;
        this.data = new Array(10);
    }
}
