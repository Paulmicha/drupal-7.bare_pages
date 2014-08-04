<?php

/**
 * @file modal.inc
 * Generic operations for modal implementations
 */

/**
 * API: Close a client side modal.
 *
 * This function should be used on form submit handlers to trigger the
 * action that will close the modal frame on the client side.
 *
 * @param $args
 *   An optional array of arguments for child pages' javascript (Drupal.settings.bare_pages.args)
 * @param $bypass_static_cache
 *   Allows overriding $args in potential subsequent calls (during the same page request)
 *
 * @see bare_pages_form_submit()
 */
function bare_pages_close_modal($args = NULL, $bypass_static_cache = FALSE) {
  // Make sure this action is not processed more than once,
  // while allowing to override $args in potential subsequent calls
  static $once;
  if (!isset($once)) {
    $once = true;
  }
  else if (!$bypass_static_cache) {
    return;
  }
  
  // Build the javascript settings that will close the modal frame on the
  // client side.
  $child_js_settings = array(
    'bare_pages' => array(
      'closeModal' => 1,
      // this seems to fail now, @see bare_pages_form_submit()
      //'statusMessages' => theme('status_messages'),
      'args' => (isset($args) && is_array($args) ? $args : array()),
    ),
  );
  drupal_add_js($child_js_settings, 'setting');
  
  // Tell Drupal's Form API that we are requested to close the modal,
  // so we do not wish to perform redirections after submitted form has been
  // processed.
  $GLOBALS['bare_pages_close_modal'] = TRUE;
}

