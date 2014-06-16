<?php

/**
 * @file theme.inc
 * Theme-related stuff
 */

/**
 * Implement hook_theme_registry_alter()
 * @see modalframe.module
 * Note 2014/06/16 21:42:39 : fixed templates suggestions
 */
function bare_pages_theme_registry_alter(&$theme_registry) {
  if (isset($theme_registry['page'])) {
    $hooks = array('page', 'html');
    $module_path = drupal_get_path('module', 'bare_pages');
    $theme_registry_copy = $theme_registry;
    _theme_process_registry($theme_registry_copy, 'phptemplate', 'theme_engine', 'pow', $module_path);
    $theme_registry += array_diff_key($theme_registry_copy, $theme_registry);
    foreach ($hooks as $h) {
      $theme_registry[$h]['theme paths'][] = $module_path;
    }
    // Add pre-preprocess function for page template.
    array_unshift($theme_registry['page']['preprocess functions'], 'bare_pages_pre_preprocess_page');
  }
}

/**
 * Preprocess template variables for page.tpl.php - step 1.
 * @see modalframe.module
 */
function bare_pages_pre_preprocess_page(&$variables) {
  if (!empty($GLOBALS['bare_pages_page_template'])) {
    $variables['show_blocks'] = FALSE;
  }
}

/**
 * Preprocess template variables for html.tpl.php
 * @see modalframe.module
 */
function bare_pages_preprocess_html(&$variables) {
  if (!empty($GLOBALS['bare_pages_page_template'])) {
    $variables['theme_hook_suggestions'][] = 'html__bare_page';
  }
}

/**
 * Preprocess template variables for page.tpl.php - step 2.
 * @see modalframe.module
 */
function bare_pages_preprocess_page(&$variables) {
  if (!empty($GLOBALS['bare_pages_page_template'])) {
    $variables['theme_hook_suggestions'][] = 'page__bare_page';
  }
}

/**
 * Implements hook_page_alter().
 * @see modalframe.module
 */
function bare_pages_page_alter(&$page) {
  if (!empty($GLOBALS['bare_pages_page_template']) && isset($page['page_top']['toolbar'])) {
    unset($page['page_top']['toolbar']);
  }
}

