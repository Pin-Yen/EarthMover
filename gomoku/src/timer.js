// Timer object
var Timer = function(span) {
  this.span = span;
  this.min = {ten: 0, one: 0};
  this.sec = {ten: 0, one: 0};
  for (var i = 0; i < 4; ++i) {
    var wrapper = span.children().eq(i);
    wrapper.find('.flip-top, .flip-bottom').text(0);
    wrapper.find('.flip-next, .flip-back').text(1);
    wrapper.find('.flip-top').removeClass('top-ani');
    wrapper.find('.flip-back').removeClass('back-ani');
  }
};

// this method would not check whether the min/sec is reasonable
Timer.prototype.set = function(min, sec) {
  if (this.sec.one != sec % 10) {
    this.sec.one = sec % 10;
    this.animate(this.span.children().eq(3), this.sec.one, this.sec.one == 9 ? 0 : this.sec.one + 1);
  }
  if (this.sec.ten != Math.floor(sec / 10)) {
    this.sec.ten = Math.floor(sec / 10);
    this.animate(this.span.children().eq(2), this.sec.ten, this.sec.ten == 5 ? 0 : this.sec.ten + 1);
  }
  if (this.min.one != min % 10) {
    this.min.one = min % 10;
    this.animate(this.span.children().eq(1), this.min.one, this.min.one == 9 ? 0 : this.min.one + 1);
  }
  if (this.min.ten != Math.floor(min / 10)) {
    this.min.ten = Math.floor(min / 10);
    this.animate(this.span.children().eq(0), this.min.ten, this.min.ten == 5 ? 0 : this.min.ten + 1);
  }
};

Timer.prototype.stop = function() {
  clearInterval(this.counting);
};

Timer.prototype.start = function() {
  var timer = this;
  this.counting = setInterval(function() { timer.add(); }, 1000);
}

// private function
Timer.prototype.add = function() {
  ++this.sec.one;
  if (this.sec.one == 10) {
    this.sec.one = 0;
    ++this.sec.ten;
    if (this.sec.ten == 6) {
      this.sec.ten = 0;
      ++this.min.one;
      if (this.min.one == 10) {
        this.min.one = 0;
        ++this.min.ten;
        this.animate(this.span.children().eq(0), this.min.ten, this.min.ten == 5 ? 0 : this.min.ten + 1);
      }
      this.animate(this.span.children().eq(1), this.min.one, this.min.one == 9 ? 0 : this.min.one + 1);
    }
    this.animate(this.span.children().eq(2), this.sec.ten, this.sec.ten == 5 ? 0 : this.sec.ten + 1);
  }
  this.animate(this.span.children().eq(3), this.sec.one, this.sec.one == 9 ? 0 : this.sec.one + 1);
};


// private function
Timer.prototype.animate = function(wrapper, num, next) {
  wrapper.find('.flip-top').addClass('top-ani');
  wrapper.find('.flip-back').addClass('back-ani');

  wrapper.find('.flip-back').on('webkitAnimationEnd oanimationend msAnimationEnd animationend', function() {
    wrapper.find('.flip-top, .flip-bottom').text(num);

    setTimeout (function() {
      wrapper.find('.flip-top').removeClass('top-ani');
      wrapper.find('.flip-back').removeClass('back-ani');

      setTimeout (function() {
        wrapper.find('.flip-next, .flip-back').text(next);
      }, 0);

    }, 0);
  });
};