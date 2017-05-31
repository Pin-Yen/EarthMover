$(function(){
  var count = 1;

  $(".top, .bottom").text(count);
  $(".next, .back").text(count == 9 ? 0 : count + 1);

  $(".back").on(
    'webkitAnimationIteration oanimationiteration msAnimationIteration animationiteration', function() {
    ++count;
    if (count == 10) count = 0;
    $(".top, .bottom").text(count);
    $(".next, .back").text(count == 9 ? 0 : count + 1);
  });
})