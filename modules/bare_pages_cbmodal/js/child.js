
/**
 * @file child.js
 * Colorbox Modal iframe pages' JS
 */

(function($) {

  /**
   * Colorbox Modal object for child windows.
   */
  Drupal.cbmodal_child = Drupal.cbmodal_child || {
    processed: false,
    behaviors: {}
  };

  /**
   * Child modal behavior.
   */
  Drupal.cbmodal_child.attach = function(context) {
    var self = Drupal.cbmodal_child;
    var settings = Drupal.settings.bare_pages || {};
    // If we cannot reach the parent window or if it doesn't have a Colorbox instance,
    // then we have nothing else todo here.
    try {
      if (!self.isObject(parent.Drupal) || !$.isFunction(parent.jQuery.colorbox)) {
        return;
      }
    }
    catch(e) {
      return;
    }
    
    // Make sure this behavior is not processed more than once.
    if (!self.processed) {
      self.processed = true;
      // If a form has been submitted successfully, then the server side script
      // may have decided to tell us the parent window to close the popup modal.
      if (settings.closeModal) {
        // Hack 2014/06/25 15:44:01 - modalframe original method for sending
        // drupal messages of child page to the parent page appears now broken
        // since the introduction of 'rebuild' to stay on the same page
        // @see bare_pages_close_modal()
        // @see bare_pages_form_submit()
        // -> ugly copy/paste workaround, for now.
        // Assumes <div id="js-messages-wrap"></div> always present in parent page theme page.tpl.php -_-
        // @todo think about this (now rushing)
        var $drupal_messages = $('.drupal-messages');
        if ($drupal_messages.length) {
          parent.jQuery('#js-messages-wrap').html('<div class="drupal-messages">' + $drupal_messages.html() + '</div>');
        }
        
        // Colorbox in parent window : close
        parent.jQuery.colorbox.close();
        
        return;
      }
    }

    // Attach child related behaviors to the current context.
    self.attachBehaviors(context);
  };


  /**
   * Check if the given variable is an object.
   */
  Drupal.cbmodal_child.isObject = function(something) {
    return (something !== null && typeof something === 'object');
  };

  /**
   * Attach child related behaviors to the iframed document.
   */
  Drupal.cbmodal_child.attachBehaviors = function(context) {
    $.each(this.behaviors, function() {
      this(context);
    });
  };


  /**
   * Add target="_blank" to all external URLs.
   */
  Drupal.cbmodal_child.behaviors.parseLinks = function(context) {
    $('a:not(.cbmodal-processed)', context).addClass('cbmodal-processed').each(function() {
      // Do not process links that have the class "cbmodal-exclude".
      if ($(this).hasClass('cbmodal-exclude')) {
        return;
      }
      // Obtain the href attribute of the link.
      var href = $(this).attr('href');
      // Do not process links with an empty href, javascript or that only have the fragment.
      if (!href || href.length <= 0 || href.charAt(0) == '#' || href.indexOf('javascript') >= 0) {
        return;
      }
      $(this).attr('target', '_blank');
    });
  };

  /**
   * Set iframe scrollbar visibility
   * Only display scrollbar if parent browser viewport height is enough
   * Hacky : if any interaction inside iframe changes the height, no way to scroll anymore...
   */
  Drupal.cbmodal_child.behaviors.RemoveIframeScrollbar = function(context) {
    // Delay a bit in case there are any post-load client-side modifications inside the iframe
    setTimeout(function() {
      
      var parent_browser_viewport_height = $(parent).height(),
        child_page_body_height = $('body').outerHeight(),
        margin = 50;
      
      if (parent_browser_viewport_height > child_page_body_height + margin) {
        
        parent.jQuery('#cboxLoadedContent .cboxIframe')
          .css({
            'height': child_page_body_height,
            //'overflow-y': 'hidden'
          })
          .attr('scrolling','no');
      }
      
    }, 30);
  };

  /**
   * Update Colorbox size to auto fit its content.
   * From documentation :
   * $.colorbox.resize() allows Colorbox to be resized based on it's own auto-calculations,
   * or to a specific size. This must be called manually after Colorbox's content has loaded.
   * The optional parameters object can accept width or innerWidth and height or innerHeight.
   * Without specifying a width or height, Colorbox will attempt to recalculate the height
   * of it's current content.
   */
  Drupal.cbmodal_child.behaviors.resizeColorbox = function(context) {
    // Delay a bit in case there are any post-load client-side modifications
    setTimeout(function() {
      parent.jQuery.colorbox.resize();
    }, 50);
  };


  /**
   * Attach our own behavior on top of the list of existing behaviors.
   *
   * We need to execute before everything else because the child frame is not
   * visible until we bind the child window to the parent, and this may cause
   * problems with other behaviors that need the document visible in order to
   * do its own job.
   */
  Drupal.behaviors.cbmodal_child = {};
  Drupal.behaviors.cbmodal_child.attach = function (context, settings) {
    Drupal.cbmodal_child.attach(context);
  }

})(jQuery);
