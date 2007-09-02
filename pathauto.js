(function($) {
  $(function() {
    // Disable input and hide its description.
    $("#edit-path").attr("disabled","disabled");
    $("//#edit-path ~ div[@class=description]").hide(0);
    $("#edit-pathauto-perform-alias").bind('click', function() {
      if ($("#edit-pathauto-perform-alias").attr("checked")) {
        // Auto-alias checked; disable input.
        $("#edit-path").attr("disabled","disabled");
        $("//#edit-path ~ div[@class=description]").slideUp('slow');
      }
      else {
        // Auto-alias unchecked; enable input.
        $("#edit-path").removeAttr("disabled");
        $("//#edit-path ~ div[@class=description]").slideDown('slow');
      }
    });
  });
})(jQuery);
