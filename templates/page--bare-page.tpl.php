<?php

/**
 * @file page--bare-page.tpl.php
 * Template file for a page rendered in Bare Pages
 * 
 * @see bare_pages_theme_registry_alter()
 * @see bare_pages_preprocess_page()
 * @see template_preprocess_page()
 * @see template_preprocess()
 * @see theme()
 */

?>
<div class="bare-page">
  <?php print $messages; ?>
  <?php print render($page['help']); ?>
  <?php print render($page['content']); ?>
</div>
