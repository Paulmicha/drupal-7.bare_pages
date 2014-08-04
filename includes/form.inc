<?php

/**
 * @file form.inc
 * Form-related stuff
 */

/**
 * Implementation of hook_form_alter().
 * @see modalframe.module
 */
function bare_pages_form_alter(&$form, &$form_state, $form_id) {
  // Here we simply want to install a form after_build callback.
  if (!isset($form['#after_build'])) {
    $form['#after_build'] = array();
  }
  $form['#after_build'][] = 'bare_pages_form_after_build';
}

/**
 * Form after build callback.
 * @see modalframe.module
 */
function bare_pages_form_after_build($form, &$form_state) {
  if (!empty($GLOBALS['bare_pages_page_template'])) {
    // Form API may have already captured submit handlers from the submitted
    // button before after_build callback is invoked. This may have been done
    // by _form_builder_handle_input_element().
    // If so, the list of submit handlers is stored in the $form_state array
    // which is something we can also alter from here, luckily. :)
    // Remember: our goal here is to make sure $form_state['redirect'] is set
    // to FALSE when the bare_pages_close_modal() API is invoked, and that's
    // because we want to tell the parent window to close the Bare Pages.
    if (!empty($form_state['submit_handlers']) && !in_array('bare_pages_form_submit', $form_state['submit_handlers'])) {
      $form_state['submit_handlers'][] = 'bare_pages_form_submit';
    }

    // Find form elements with submit handlers recursively.
    bare_pages_fapiu_ashr($form, $form_state, 'bare_pages_form_submit');
  }
  return $form;
}

/**
 * Generic Helper : adds a submit handler anywhere in the $form
 * (fapiu_ashr = Form API Utility : Add Submit Handler Recursively)
 */
function bare_pages_fapiu_ashr(&$elements, &$form_state, $submit_handler) {
  // Recurse through all children elements.
  foreach (element_children($elements) as $key) {
    if (isset($elements[$key]) && $elements[$key]) {
      bare_pages_fapiu_ashr($elements[$key], $form_state, $submit_handler);
    }
  }
  // If this element has submit handlers, then append our own.
  if (isset($elements['#submit'])) {
    $elements['#submit'][] = $submit_handler;
  }
}

/**
 * Generic form submit handler.
 *
 * When we are requested to close a modal dialog, we don't want Form API to
 * perform any redirection once the submitted form has been processed.
 *
 * @see modalframe.module
 * @see drupal_redirect_form()
 */
function bare_pages_form_submit($form, &$form_state) {
  if (!empty($GLOBALS['bare_pages_close_modal'])) {
    // Hacky, but I don't see how to circumvent the "?destination=path" in any other way
    unset($_GET['destination']);
    $form_state['redirect'] = FALSE;// This seems to fail (on its own ?) -> using 'rebuild' as well
    $form_state['rebuild'] = TRUE;
  }
}

