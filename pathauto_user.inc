<?php
// $Id$

/**
 * Implementation of hook_pathauto() for user aliases
 */
function user_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'user';
      $settings['groupheader'] = t('User path settings');
      $settings['patterndescr'] = t('Pattern for user account page paths');
      $settings['patterndefault'] = t('users/[user]');
      $patterns = token_get_list('user');
      foreach ($patterns['user'] as $pattern => $description) {
        $settings['placeholders'][t('['. $pattern .']')] = t($description);
      }
      $settings['bulkname'] = t('Bulk update user paths');
      $settings['bulkdescr'] = t('Generate aliases for all existing user account pages which do not already have aliases.');
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_pathauto() for blog aliases
 */
function blog_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'blog';
      $settings['groupheader'] = t('Blog path settings');
      $settings['patterndescr'] = t('Pattern for blog page paths');
      $settings['patterndefault'] = t('blogs/[user]');
      $patterns = module_invoke_all('token_list', 'user');
      foreach ($patterns['user'] as $pattern => $description) {
        $settings['placeholders'][t('['. $pattern .']')] = t($description);
      }
      $settings['supportsfeeds'] = 'feed';
      $settings['bulkname'] = t('Bulk update blog paths');
      $settings['bulkdescr'] = t('Generate aliases for all existing blog pages which do not already have aliases. Note: Bulk Update may not complete on large or slow sites. See the README.txt for more information.');
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_pathauto() for user-tracker aliases
 */
function tracker_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'tracker';
      $settings['groupheader'] = t('User-tracker path settings');
      $settings['patterndescr'] = t('Pattern for user-tracker page paths');
      $settings['patterndefault'] = t('users/[user]/track');
      $patterns = module_invoke_all('token_list', 'user');
      foreach ($patterns['user'] as $pattern => $description) {
        $settings['placeholders'][t('['. $pattern .']')] = t($description);
      }
      $settings['supportsfeeds'] = 'feed';
      $settings['bulkname'] = t('Bulk update user-tracker paths');
      $settings['bulkdescr'] = t('Generate aliases for all existing user-tracker pages which do not already have aliases.');
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_user() for users, trackers, and blogs
 */
function pathauto_user($op, &$edit, &$user, $category = FALSE) {
  switch ($op) {
    case 'insert':
    case 'update':
      /*
      ** Use the username to automatically create an alias
      */
      $user = (object) $user;
      if ($user->name) {
        $placeholders = pathauto_get_placeholders('user', $user);
        $src = 'user/'.$user->uid;
        $alias = pathauto_create_alias('user', $op, $placeholders, $src);
        if (module_exists('blog')) {
          $src = 'blog/'.$user->uid;
          $alias = pathauto_create_alias('blog', $op, $placeholders, $src);
        }
        if (module_exists('tracker')) {
          $src = 'user/'.$user->uid.'/track';
          $alias = pathauto_create_alias('tracker', $op, $placeholders, $src);
        }
      }
      break;
    case 'delete':
      /*
      ** If the user is deleted, remove the path aliases
      **
      */
      $user = (object) $user;
      path_set_alias('user/'.$user->uid);

      //  They may have enabled these modules and/or feeds when the user was created, so let's try to delete all of them
      path_set_alias('blog/'.$user->uid);
      path_set_alias('blog/'. $user->uid .'/feed');
      path_set_alias('user/'.$user->uid.'/track');
      path_set_alias('user/'.$user->uid.'/track/feed');

      break;
    default:
      break;
  }
}

/**
 * Bulk generate aliases for all users without aliases
 */
function user_pathauto_bulkupdate() {
  $query = "SELECT uid,name,src,dst FROM {users} LEFT JOIN {url_alias} ON CONCAT('user/', uid) = src WHERE uid > 0";
  $result = db_query($query);
  
  $count = 0;
  $placeholders = array();
  while ($user = db_fetch_object($result)) {
    $placeholders = pathauto_get_placeholders('user', $user);        
    $src = 'user/'.$user->uid;
    if ($alias = pathauto_create_alias('user', 'bulkupdate', $placeholders, $src)) {
      $count++;
    }
  }
  
  drupal_set_message(format_plural($count,
    "Bulk update of users completed, one alias generated.",
    "Bulk update of users completed, @count aliases generated."));
}


/**
 * Bulk generate aliases for all blogs without aliases 
 */
function blog_pathauto_bulkupdate() {
  $query = "SELECT uid,name,src,dst FROM {users} LEFT JOIN {url_alias} ON CONCAT('user/', uid) = src WHERE uid > 0";
  $result = db_query($query);
  $user = db_fetch_object($result);
  
  $count = 0;
  $placeholders = array();
  while ($user) {
    $placeholders = pathauto_get_placeholders('user', $user);
    $src = 'blog/'.$user->uid;
    if ($alias = pathauto_create_alias('blog', 'bulkupdate', $placeholders, $src)) {
      $count++;
    }
    $user = db_fetch_object($result);
  }
  
  drupal_set_message(format_plural($count,
    "Bulk update of user blogs completed, one alias generated.",
    "Bulk update of user blogs completed, @count aliases generated."));
}

/**
 * Bulk generate aliases for user trackers without aliases 
 */
function tracker_pathauto_bulkupdate() {
  $query = "SELECT uid,name,src,dst FROM {users} LEFT JOIN {url_alias} ON CONCAT('user/', uid) = src WHERE uid > 0";
  $result = db_query($query);
  $user = db_fetch_object($result);
  
  $count = 0;
  $placeholders = array();
  while ($user) {
    $placeholders = pathauto_get_placeholders('user', $user);
    $src = 'user/'.$user->uid.'/track';
    if ($alias = pathauto_create_alias('tracker', 'bulkupdate', $placeholders, $src)) {
      $count++;
    }
    $user = db_fetch_object($result);
  }
  
  drupal_set_message(format_plural($count,
    "Bulk update of user tracker pages completed, one alias generated.",
    "Bulk update of user tracker pages completed, @count aliases generated."));
}
