<?php
// $Id$

/**
 * @file
 * Hook implementations for node module integration.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_pathauto().
 */
function node_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'node';
      $settings['token_type'] = 'node';
      $settings['groupheader'] = t('Node paths');
      $settings['patterndescr'] = t('Default path pattern (applies to all node types with blank patterns below)');
      $settings['patterndefault'] = t('content/[title-raw]');
      $settings['batch_update_callback'] = 'node_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto_node.inc';
      $settings['supportsfeeds'] = 'feed';

      $languages = array();
      if (module_exists('locale')) {
        $languages = array('' => t('language neutral')) + locale_language_list('name');
      }

      foreach (node_get_types('names') as $node_type => $node_name) {
        if (count($languages) && variable_get('language_content_type_'. $node_type, 0)) {
          $settings['patternitems'][$node_type] = t('Default path pattern for @node_type (applies to all @node_type node types with blank patterns below)', array('@node_type' => $node_name));
          foreach ($languages as $lang_code => $lang_name) {
            $settings['patternitems'][$node_type . '_' . $lang_code] = t('Pattern for all @language @node_type paths', array('@node_type' => $node_name, '@language' => $lang_name));
          }
        }
        else {
          $settings['patternitems'][$node_type] = t('Pattern for all @node_type paths', array('@node_type' => $node_name));
        }
      }
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Batch processing callback; Generate aliases for nodes.
 */
function node_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $sql = "SELECT n.nid FROM {node} n LEFT JOIN {url_alias} ua ON CONCAT('node/', CAST(n.nid AS CHAR)) = ua.src WHERE ua.src IS NULL AND n.nid > %d ORDER BY n.nid";
  $args = array($context['sandbox']['current']);

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $count_sql = str_replace('SELECT n.nid', 'SELECT COUNT(n.nid)', $sql);
    $context['sandbox']['total'] = db_result(db_query($count_sql, $args));

    // If there are no nodes to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query = db_query_range($sql, $args, 0, 5);
  $nids = array();
  while ($nid = db_result($query)) {
    $nids[] = $nid;
  }

  pathauto_node_update_alias_multiple($nids, 'bulkupdate');
  $context['sandbox']['count'] += count($nids);
  $context['sandbox']['current'] = max($nids);
  $context['message'] = t('Updated alias for node @nid.', array('@nid' => end($nids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}
