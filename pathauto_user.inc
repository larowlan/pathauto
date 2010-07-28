<?php
// $Id$

/**
 * @file
 * Hook implementations for user module integration.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_pathauto().
 */
function user_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'user';
      $settings['token_type'] = 'user';
      $settings['groupheader'] = t('User paths');
      $settings['patterndescr'] = t('Pattern for user account page paths');
      $settings['patterndefault'] = t('users/[user-raw]');
      $settings['batch_update_callback'] = 'user_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto_user.inc';
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
function blog_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'blog';
      $settings['token_type'] = 'user';
      $settings['groupheader'] = t('Blog paths');
      $settings['patterndescr'] = t('Pattern for blog page paths');
      $settings['patterndefault'] = t('blogs/[user-raw]');
      $settings['supportsfeeds'] = 'feed';
      $settings['batch_update_callback'] = 'blog_pathauto_bulk_update_batch_process';
      $settings['batch_file'] = drupal_get_path('module', 'pathauto') . '/pathauto_user.inc';
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
