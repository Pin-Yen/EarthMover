$(function(){
  var count = 0;

  set(1);

  setInterval(add, 1000);

  function set(val) {
    count = val;
    $(".top, .bottom").text(count == 0 ? 9 : count - 1);
    $(".next, .back").text(count);
    animate();
  }

  function add() {
    ++count;
    if (count == 10) count = 0;

    animate();
  }

  function animate() {
    $(".top").addClass('top-ani');
    $(".back").addClass('back-ani');

    $(".back").on('webkitAnimationEnd oanimationend msAnimationEnd animationend', function() {
      $(".top, .bottom").text(count);

      setTimeout (function() {
        $(".top").removeClass('top-ani');
        $(".back").removeClass('back-ani');

        setTimeout (function() {
          $(".next, .back").text(count == 9 ? 0 : count + 1);
        }, 0);

      }, 0);
    });
  }
})