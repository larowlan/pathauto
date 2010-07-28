<?php
// $Id$

/**
 * @file
 * Hook implementations for taxonomy module integration.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_pathauto().
 */
function taxonomy_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'taxonomy';
      $settings['token_type'] = 'taxonomy';
      $settings['groupheader'] = t('Taxonomy term paths');
      $settings['patterndescr'] = t('Default path pattern (applies to all vocabularies with blank patterns below)');
      $settings['patterndefault'] = t('category/[vocab-raw]/[catpath-raw]');
      $settings['supportsfeeds'] = '0/feed';
      $settings['batch_update_callback'] = 'taxonomy_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto_taxonomy.inc';

      $vocabularies = taxonomy_get_vocabularies();
      if (sizeof($vocabularies) > 0) {
        $settings['patternitems'] = array();
        $forum_vid = variable_get('forum_nav_vocabulary', '');
        foreach ($vocabularies as $vocab) {
          if ($vocab->vid != $forum_vid) {
            $vocabname = $vocab->name;
            $fieldlabel = t('Pattern for all %vocab-name paths', array('%vocab-name' => $vocabname));
            $settings['patternitems'][$vocab->vid] = $fieldlabel;
          }
        }
      }
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Batch processing callback; Generate aliases for taxonomy terms.
 */
function taxonomy_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $forum_vid = variable_get('forum_nav_vocabulary', '');
  $sql = "SELECT t.tid FROM {term_data} t LEFT JOIN {url_alias} ua ON CONCAT('taxonomy/term/', CAST(t.tid AS CHAR)) = ua.src WHERE ua.src IS NULL AND t.tid > %d AND t.vid <> %d ORDER BY t.tid";
  $args = array($context['sandbox']['current'], $forum_vid);

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $count_sql = str_replace('SELECT t.tid', 'SELECT COUNT(t.tid)', $sql);
    $context['sandbox']['total'] = db_result(db_query($count_sql, $args));

    // If there are no nodes to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query = db_query_range($sql, $args, 0, 25);
  $tids = array();
  while ($tid = db_result($query)) {
    $tids[] = $tid;
  }

  pathauto_taxonomy_term_update_alias_multiple($tids, 'bulkupdate');
  $context['sandbox']['count'] += count($tids);
  $context['sandbox']['current'] = max($tids);
  $context['message'] = t('Updated alias for term @tid.', array('@tid' => end($tids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}

/**
 * Implementation of hook_pathauto() for forum module.
 */
function forum_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'forum';
      $settings['token_type'] = 'taxonomy';
      $settings['groupheader'] = t('Forum paths');
      $settings['patterndescr'] = t('Pattern for forums and forum containers');
      $settings['patterndefault'] = t('[vocab-raw]/[catpath-raw]');
      $settings['supportsfeeds'] = '0/feed';
      $settings['batch_update_callback'] = 'forum_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto_taxonomy.inc';
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Batch processing callback; Generate aliases for forums.
 */
function forum_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $forum_vid = variable_get('forum_nav_vocabulary', '');
  $sql = "SELECT t.tid FROM {term_data} t LEFT JOIN {url_alias} ua ON CONCAT('forum/', CAST(t.tid AS CHAR)) = ua.src WHERE ua.src IS NULL AND t.tid > %d AND t.vid = %d ORDER BY t.tid";
  $args = array($context['sandbox']['current'], $forum_vid);

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $count_sql = str_replace('SELECT t.tid', 'SELECT COUNT(t.tid)', $sql);
    $context['sandbox']['total'] = db_result(db_query($count_sql, $args));

    // If there are no nodes to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query = db_query_range($sql, $args, 0, 25);
  $tids = array();
  while ($tid = db_result($query)) {
    $tids[] = $tid;
  }

  pathauto_taxonomy_term_update_alias_multiple($tids, 'bulkupdate');
  $context['sandbox']['count'] += count($tids);
  $context['sandbox']['current'] = max($tids);
  $context['message'] = t('Updated alias for forum @tid.', array('@tid' => end($tids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}
