<?php

/**
 * @file
 * Pathauto integration for core modules.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_path_alias_types().
 *
 * Used primarily by the bulk delete form.
 */
function pathauto_path_alias_types() {
  $objects['user/'] = t('Users');
  $objects['node/'] = t('Content');
  if (\Drupal::moduleHandler()->moduleExists('taxonomy')) {
    $objects['taxonomy/term/'] = t('Taxonomy terms');
  }
  if (\Drupal::moduleHandler()->moduleExists('forum')) {
    $objects['forum/'] = t('Forums');
  }
  return $objects;
}
