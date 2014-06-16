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
    // to FALSE when the bare_pages_close_dialog() API is invoked, and that's
    // because we want to tell the parent window to close the Bare Pages.
    if (!empty($form_state['submit_handlers']) && !in_array('bare_pages_form_submit', $form_state['submit_handlers'])) {
      $form_state['submit_handlers'][] = 'bare_pages_form_submit';
    }

    // Find form elements with submit handlers recursively.
    bare_pages_form_after_build_recursive($form, $form_state);
  }
  return $form;
}

/**
 * Find form elements with submit handlers recursively.
 * @see modalframe.module
 */
function bare_pages_form_after_build_recursive(&$elements, &$form_state) {
  // Recurse through all children elements.
  foreach (element_children($elements) as $key) {
    if (isset($elements[$key]) && $elements[$key]) {
      bare_pages_form_after_build_recursive($elements[$key], $form_state);
    }
  }
  // If this element has submit handlers, then append our own.
  if (isset($elements['#submit'])) {
    $elements['#submit'][] = 'bare_pages_form_submit';
  }
}

/**
 * Generic form submit handler.
 *
 * When we are requested to close a modal dialog, we don't want Form API to
 * perform any redirection once the submitted form has been processed.
 *
 * When $form_state['redirect'] is set to FALSE, then Form API will simply
 * re-render the form with the values still in its fields. And this is all
 * we need to output the javascript that will tell the parent window to close
 * the child dialog.
 *
 * @ingroup forms
 * @see modalframe.module
 */
function bare_pages_form_submit($form, &$form_state) {
  if (!empty($GLOBALS['bare_pages_close_dialog'])) {
    $form_state['redirect'] = FALSE;
  }
}

