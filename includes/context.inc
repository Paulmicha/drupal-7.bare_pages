<?php

/**
 * @file context.inc
 * Determine & react to custom parent or child "contexts"
 */

/**
 * Implement hook_init()
 */
function bare_pages_init() {
  $active_context = bare_pages_get_context();
  if ($active_context == 'child') {
    bare_pages_set_child_context();
  }
  else if ($active_context == 'parent') {
    bare_pages_set_parent_context();
  }
}


/**
 * Get active "context"
 * @param [optional] Boolean $rebuild : bypass statically cached conditions checks
 * @return String 'child' | 'parent'
 */
function bare_pages_get_context($rebuild = FALSE) {
  $context = &drupal_static(__FUNCTION__);
  if (!isset($context) && !$rebuild) {
    if (bare_pages_context_conditions_check('child')) {
      $context = 'child';
    }
    else if (bare_pages_context_conditions_check('parent')) {
      $context = 'parent';
    }
  }
  return $context;
}

/**
 * Sets "parent" context
 * Typically : for a page containing a modal
 */
function bare_pages_set_parent_context() {
  static $processed;
  // Make sure external resources are not included more than once.
  if (isset($processed)) {
    return;
  }
  $processed = TRUE;
  
  // Allow other modules to react
  module_invoke_all('bp_parent_context_activated');
}

/**
 * Sets "child" context
 * Typically : for a page contained inside a modal
 */
function bare_pages_set_child_context() {
  static $processed;
  // Make sure external resources are not included more than once.
  if (isset($processed)) {
    return;
  }
  $processed = TRUE;

  // Disable admin_menu, admin module output and similar modules, which
  // is something child windows don't need.
  module_invoke_all('suppress');

  // Tell Drupal's theme system to use the Bare Pages template.
  $GLOBALS['bare_pages_page_template'] = TRUE;
  
  // Allow other modules to react
  module_invoke_all('bp_child_context_activated');
}


/**
 * Context conditions checks
 *
 * Default behavior is looking for a GET param,
 * but other modules can provide other means of detection.
 * Note : this can potentially be called at various moments (hook_init, etc.)
 *
 * @param String $type : 'child' or 'parent'
 * @return Boolean
 */
function bare_pages_context_conditions_check($type) {
  // Failsafe
  if ($type != 'child' && $type != 'parent') {
    return false;
  }
  
  // Other modules' custom "context" detection
  $bool_arr = array();
  foreach (module_implements('detect_bp_' . $type . '_context') as $module) {
    $bool_arr[] = module_invoke($module, 'detect_bp_' . $type . '_context');
  }
  
  // Default detection is OR
  // (means anyone saying true "wins")
  // (if it was AND, anyone saying false "wins")
  // @todo configurable custom settings form for conditions checking method (AND)
  if (!empty($bool_arr)) {
    $checking_method = variable_get('bp_checking_method', 'OR');
    foreach($bool_arr as $bool) {
      if ($checking_method == 'OR' && $bool) {
        return true;
      }
      if ($checking_method == 'AND' && !$bool) {
        return false;
      }
    }
  }
  
  // Default behavior (when no other modules provide detection)
  else {
    // @todo configurable custom settings form for param
    $var_name = variable_get('bp_' . $type . '_get_param_name', 'b' . $type);
    if (!empty($_GET[$var_name])) {
      return true;
    }
  }
  
  return false;
}

