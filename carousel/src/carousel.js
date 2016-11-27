$(document).ready(function() {

  // Image collection.
  var carouselImages = [
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Linux-Main.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Linux-Group.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Linux-Inv.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Linux-Fish.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Mac-Settings.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Mac-Main.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Mac-Group.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Mac-Inv.png",
    "https://github.com/madsciencecoder/Shticker-Book-Rewritten/raw/gh-pages/images/Mac-Fish.png"
  ];

  var totalCarouselImages = carouselImages.length;

  /**
  * Images are added to the carousel here.
  * Three images (active, right and left) are added, relative to the index of   * the currently active image.
  *
  * @param {Integer} activeImgIndex
  * @return {String} imgElements
  */
  function carouselInnerHTML(activeImgIndex) {

    var imgActiveSrc = carouselImages[activeImgIndex];
    var imgRightSrc;
    var imgLeftSrc;
    var imgElements;

    /** If at start, set the last image in the collection as the left/previous  * image.
    */
    if (activeImgIndex === 0) {
      imgLeftSrc = carouselImages[totalCarouselImages - 1];
    } else {
      imgLeftSrc = carouselImages[activeImgIndex - 1];
    }

    // If at end, set the first image in the collection as the right/next image.
    if (activeImgIndex === totalCarouselImages - 1) {
      imgRightSrc = carouselImages[0];
    } else {
      imgRightSrc = carouselImages[activeImgIndex + 1];
    }

    imgElements = '<img src="' + imgLeftSrc + '" class="img-left">';
    imgElements += '<img src="' + imgActiveSrc + '" class="img-active"';
    imgElements += 'data-active-index="' + activeImgIndex + '">'
    imgElements += '<img src="' + imgRightSrc + '" class="img-right">';

    return imgElements;
  }

  // Start carousel on page load.
  $('.carousel-images').html( carouselInnerHTML(0) );

  /**
  * Get indicators based on size of image collection.
  *
  * @return {String} indicatorDots
  */
  function carouselIndicators() {
    var indicatorDots = '';
    for (var i = 0; i < totalCarouselImages; i++) {
      indicatorDots += '<div></div>';
    }
    return indicatorDots;
  }

  // Draw carousel indicators.
  $('.carousel-indicators').html( carouselIndicators() );
  $('.carousel-indicators > div').eq(0).addClass('active');

  /**
  * Depending on advance direction, get index for generating the next set of    * three images. And change color of indicators.
  *
  * @param {String} advanceDirection
  * @return {Integer} nextIndex
  */
  function nextThree(advanceDirection) {

    var currentActiveIndex = $('.img-active').data('active-index');
    var nextIndex;

    if (advanceDirection === 'right') {
      nextIndex = currentActiveIndex === totalCarouselImages - 1 ? 0 : currentActiveIndex + 1;
    }

    if (advanceDirection === 'left') {
      nextIndex = currentActiveIndex === 0 ? totalCarouselImages - 1 : currentActiveIndex - 1;
    }

    $('.carousel-indicators > div').eq(currentActiveIndex).removeClass('active');
    $('.carousel-indicators > div').eq(nextIndex).addClass('active');

    return nextIndex;
  }

  // Attach events to Left and Right buttons.
  function rightLeftHandler() {
    $('.advance-right').bind('click', advanceRight);
    $('.advance-left').bind('click', advanceLeft);
  }

  rightLeftHandler();

  // Advancing slides right.
  function advanceRight() {
    $('.advance-right, .advance-left').unbind('click');
    $('.img-active').addClass('slide-out-left');
    $('.img-right').addClass('slide-in-left');

    // Wait for CSS animation to end, before getting new images.
    var updateCarousel = nextThree('right');
    $('.slide-out-left').on('animationend', function() {
      $('.carousel-images').html( carouselInnerHTML( updateCarousel ) );
      rightLeftHandler();
    });
  }

  // Advance Left.
  function advanceLeft() {
    $('.advance-left, .advance-right').unbind('click');
    $('.img-active').addClass('slide-out-right');
    $('.img-left').addClass('slide-in-right');

    var updateCarousel = nextThree('left');
    $('.slide-out-right').on('animationend', function() {
      $('.carousel-images').html( carouselInnerHTML( updateCarousel ) );
      rightLeftHandler();
    });
  }

  // Use indicators to jump between slides.
  $('.carousel-indicators > div').click(function() {
    $('.carousel-indicators').find('.active').removeClass();
    $(this).addClass('active');
    $('.carousel-images').html( carouselInnerHTML( $(this).index() ));
  });

  // Auto change slides every 3 seconds. Stop timer on hover.
  var autoAdvance = setInterval(advanceRight, 6000);
  $('.carousel-container').hover(
    function() {
      clearInterval(autoAdvance);
    }, function() {
      autoAdvance = setInterval(advanceRight, 6000);
    }
  );

});
