<?php
// $Id$

/*
 * Implementation of hook_pathauto()
 */

function taxonomy_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'taxonomy';
      $settings['groupheader'] = t('Category path settings');
      $settings['patterndescr'] = t('Default path pattern (applies to all vocabularies with blank patterns below)');
      $settings['patterndefault'] = t('[vocab]/[catpath]');
      $settings['placeholders'] = array(
        t('[vocab]') => t("The vocabulary that the page's first category belongs to."),
        t('[cat]') => t('The name of the category.'),
        t('[catpath]') => t('As [cat], but including its supercategories.'),
        t('[tid]') => t('The id number of the category.'),
      );
      // Look for extensions from other modules
      $placeholders = module_invoke_all('pathauto_taxonomy', 'placeholders');
      $settings['placeholders'] = array_merge($settings['placeholders'], $placeholders);
      $settings['supportsfeeds'] = '0/feed';
      $settings['bulkname'] = t('Bulk update category paths');
      $settings['bulkdescr'] = t('Generate aliases for all existing categories which do not already have aliases.');

      $vocabularies = taxonomy_get_vocabularies();
      if (sizeof($vocabularies) > 0) {
        $settings['patternitems'] = array();
        foreach ($vocabularies as $vocab) {
          $vocabname = $vocab->name;
          $fieldlabel = t('Pattern for all %vocab-name paths', array('%vocab-name' => $vocabname));
          $settings['patternitems'][$vocab->vid] = $fieldlabel;
        }
      }
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_taxonomy().
 */
function pathauto_taxonomy($op, $type, $array = NULL) {
  switch ($type) {
    case 'term':
      switch ($op) {
        case 'insert':
        case 'update':
          // Use the category info to automatically create an alias
          $category = (object) $array;

          // Work-around for the different structure of the third parameter
          // between Drupal 4.7.x and 5.x hook_taxonomy(). This allows us to
          // use a single, clean catpath helper function.
          $category->parent = $category->parent[0];
          if ($category->name) {
            // Generate the placeholders for this vocabulary.
            $placeholders = array();            
            $vid = $category->vid;
            $vocabulary = taxonomy_get_vocabulary($vid);
            $placeholders[t('[vocab]')] = pathauto_cleanstring($vocabulary->name);
            $placeholders[t('[cat]')] = pathauto_cleanstring($category->name);
            $placeholders[t('[tid]')] = $category->tid;
            
            $catpath = ''; 
            $placeholders[t('[catpath]')] = _pathauto_taxonomy_catpath($category);
            
            // Append any additional extensions
            $extplaceholders = module_invoke_all('pathauto_taxonomy', 'values', $category);
            $placeholders = array_merge($placeholders, $extplaceholders);
            
            // Create the taxonomy term alias.
            $src = taxonomy_term_path($category);
            $alias = pathauto_create_alias('taxonomy', $op, $placeholders, $src, $vid);

            // If we're in a forum vocabulary, also create a forum container, forum, or forum topic alias.
            if (module_exists('forum')) {
              $forumvid = variable_get('forum_nav_vocabulary', '');  
              if ($vid == $forumvid) {
                $src = 'forum/'. $category->tid;
                $alias = pathauto_create_alias('forum', $op, $placeholders, $src, $vid);
              }
            }
          }
          break;
        case 'delete':
          //If the category is deleted, remove the path aliases
          $category = (object) $object;
          path_set_alias('taxonomy/term/'. $category->tid);
          path_set_alias('forum/'. $category->tid);
          break;
        default:
          break;
      }

      break;
    default:
      break;
  }
}

/**
 * Generate aliases for all categories without aliases
 */
function taxonomy_pathauto_bulkupdate() {
  $query = "SELECT tid,vid,name,src,dst FROM {term_data} LEFT JOIN {url_alias} ON CONCAT('taxonomy/term/', tid) = src";
  $result = db_query($query);
  $category = db_fetch_object($result);
  if (module_exists('forum')) {
    $forumvid = variable_get('forum_nav_vocabulary', '');  
  }
  
  $count = 0;
  $placeholders = array();
  while ($category) {
    $vid = $category->vid;
    if (!module_exists($forum) || (module_exists($forum) && $forumvid != $vid)) {
      $vocabulary = taxonomy_get_vocabulary($vid);
      $placeholders[t('[vocab]')] = pathauto_cleanstring($vocabulary->name);
      $placeholders[t('[cat]')] = pathauto_cleanstring($category->name);
      $placeholders[t('[tid]')] = $category->tid;
      $placeholders[t('[catpath]')] = _pathauto_taxonomy_catpath($category);
    
      // Append any additional extensions
      $extplaceholders = module_invoke_all('pathauto_taxonomy', 'values', $category);
      $placeholders = array_merge($placeholders, $extplaceholders);

      $src = 'taxonomy/term/'. $category->tid;
      if ($alias = pathauto_create_alias('taxonomy', 'bulkupdate', $placeholders, $src, $vid)) {
        $count++;
      }
    }
    $category = db_fetch_object($result);
  }
  
  drupal_set_message(format_plural($count,
    "Bulk update of terms completed, one alias generated.",
    "Bulk update of terms completed, @count aliases generated."));
}

/**
 * Implementation of hook_pathauto() for forum topics.
 */
function forum_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'forum';
      $settings['groupheader'] = t('Forum path settings');
      $settings['patterndescr'] = t('Pattern for forums and forum containers');
      $settings['patterndefault'] = t('[vocab]/[catpath]');
      $settings['placeholders'] = array(
        t('[vocab]') => t("The vocabulary that the page's first category belongs to."),
        t('[cat]') => t('The name of the category.'),
        t('[catpath]') => t('As [cat], but including its supercategories.'),
        t('[tid]') => t('The id number of the category.'),
      );
      $settings['supportsfeeds'] = '0/feed';
      $settings['bulkname'] = t('Bulk update forum paths');
      $settings['bulkdescr'] = t('Generate aliases for all existing forums and forum containers which do not already have aliases.');
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Generate aliases for all forums and forum containers without aliases
 */
function forum_pathauto_bulkupdate() {
  $query = "SELECT tid,vid,name,src,dst FROM {term_data} LEFT JOIN {url_alias} ON CONCAT('forum/', tid) = src";
  $result = db_query($query);
  $category = db_fetch_object($result);

  $count = 0;
  $placeholders = array();
  while ($category) {
    $vid = $category->vid;
    if ($vid == _forum_get_vid()) {
      $vocabulary = taxonomy_get_vocabulary($vid);
      $placeholders[t('[vocab]')] = pathauto_cleanstring($vocabulary->name);
      $placeholders[t('[cat]')] = pathauto_cleanstring($category->name);
      $placeholders[t('[tid]')] = $category->tid;
      $placeholders[t('[catpath]')] = _pathauto_taxonomy_catpath($category);
    
      // Append any additional extensions
      $extplaceholders = module_invoke_all('pathauto_taxonomy', 'values', $category);
      $placeholders = array_merge($placeholders, $extplaceholders);
    
      $src = 'forum/'. $category->tid;
      if ($alias = pathauto_create_alias('forum', 'bulkupdate', $placeholders, $src, $vid)) {
        $count++;
      }
    }
    $category = db_fetch_object($result);
  }
  
  drupal_set_message(format_plural($count,
    "Bulk update of forums and forum containers completed, one alias generated.",
    "Bulk update of forums and forum containers completed, @count aliases generated."));
}


/**
 * Helper function for the [catpath] placeholder.
 */
function _pathauto_taxonomy_catpath($category) {
  if ($category->parent) {
    $catpath = pathauto_cleanstring($category->name);
    $parents = taxonomy_get_parents_all($category->parent);
  } 
  else {
    $catpath = '';
    $parents = taxonomy_get_parents_all($category->tid);
  }
  foreach ($parents as $parent) {
    $catpath = pathauto_cleanstring($parent->name) .'/'. $catpath;
  }
  return $catpath;
}
