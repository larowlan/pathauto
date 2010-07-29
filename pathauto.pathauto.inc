<?php
// $Id$

/**
 * @file
 * Pathauto integration for core modules.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_pathauto().
 */
function _node_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'node';
      $settings['token_type'] = 'node';
      $settings['groupheader'] = t('Node paths');
      $settings['patterndescr'] = t('Default path pattern (applies to all node types with blank patterns below)');
      $settings['patterndefault'] = 'content/[title-raw]';
      $settings['batch_update_callback'] = 'node_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto.pathauto.inc';
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

/**
 * Implements hook_pathauto().
 */
function _taxonomy_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'taxonomy';
      $settings['token_type'] = 'taxonomy';
      $settings['groupheader'] = t('Taxonomy term paths');
      $settings['patterndescr'] = t('Default path pattern (applies to all vocabularies with blank patterns below)');
      $settings['patterndefault'] = '[vocab-raw]/[catpath-raw]';
      $settings['supportsfeeds'] = '0/feed';
      $settings['batch_update_callback'] = 'taxonomy_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto.pathauto.inc';

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
 * Implementation of hook_pathauto().
 */
function _forum_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'forum';
      $settings['token_type'] = 'taxonomy';
      $settings['groupheader'] = t('Forum paths');
      $settings['patterndescr'] = t('Pattern for forums and forum containers');
      $settings['patterndefault'] = '[vocab-raw]/[catpath-raw]';
      $settings['supportsfeeds'] = '0/feed';
      $settings['batch_update_callback'] = 'forum_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto.pathauto.inc';
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

/**
 * Implements hook_pathauto().
 */
function _user_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'user';
      $settings['token_type'] = 'user';
      $settings['groupheader'] = t('User paths');
      $settings['patterndescr'] = t('Pattern for user account page paths');
      $settings['patterndefault'] = 'users/[user-raw]';
      $settings['batch_update_callback'] = 'user_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto.pathauto.inc';
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Batch processing callback; Generate aliases for users.
 */
function user_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $sql = "SELECT u.uid FROM {users} u LEFT JOIN {url_alias} ua ON CONCAT('user/', CAST(u.uid AS CHAR)) = ua.src WHERE ua.src IS NULL AND u.uid > %d ORDER BY u.uid";
  $args = array($context['sandbox']['current']);

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $count_sql = str_replace('SELECT u.uid', 'SELECT COUNT(u.uid)', $sql);
    $context['sandbox']['total'] = db_result(db_query($count_sql, $args));

    // If there are no nodes to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query = db_query_range($sql, $args, 0, 25);
  $uids = array();
  while ($uid = db_result($query)) {
    $uids[] = $uid;
  }

  pathauto_user_update_alias_multiple($uids, 'bulkupdate', array('alias blog' => FALSE));
  $context['sandbox']['count'] += count($uids);
  $context['sandbox']['current'] = max($uids);
  $context['message'] = t('Updated alias for user @uid.', array('@uid' => end($uids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}

/**
 * Implements hook_pathauto().
 */
function _blog_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'blog';
      $settings['token_type'] = 'user';
      $settings['groupheader'] = t('Blog paths');
      $settings['patterndescr'] = t('Pattern for blog page paths');
      $settings['patterndefault'] = 'blogs/[user-raw]';
      $settings['supportsfeeds'] = 'feed';
      $settings['batch_update_callback'] = 'blog_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto.pathauto.inc';
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Batch processing callback; Generate aliases for blogs.
 */
function blog_pathauto_bulk_update_batch_process(&$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  $sql = "SELECT u.uid FROM {users} u LEFT JOIN {url_alias} ua ON CONCAT('blog/', CAST(u.uid AS CHAR)) = ua.src WHERE ua.src IS NULL AND u.uid > %d ORDER BY u.uid";
  $args = array($context['sandbox']['current']);

  // Get the total amount of items to process.
  if (!isset($context['sandbox']['total'])) {
    $count_sql = str_replace('SELECT u.uid', 'SELECT COUNT(u.uid)', $sql);
    $context['sandbox']['total'] = db_result(db_query($count_sql, $args));

    // If there are no nodes to update, the stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  $query = db_query_range($sql, $args, 0, 25);
  $uids = array();
  while ($uid = db_result($query)) {
    $uids[] = $uid;
    $account = user_load($uid);
    pathauto_blog_update_alias($account, 'bulkupdate');
  }

  $context['sandbox']['count'] += count($uids);
  $context['sandbox']['current'] = max($uids);
  $context['message'] = t('Updated alias for blog user @uid.', array('@uid' => end($uids)));

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}
