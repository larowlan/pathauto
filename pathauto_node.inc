<?php
// $Id$

/*
 * Implementation of hook_pathauto()
 */
function node_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'node';
      $settings['groupheader'] = t('Node path settings');
      $settings['patterndescr'] = t('Default path pattern (applies to all node types with blank patterns below)');
      $settings['patterndefault'] = t('content/[title]');
      $settings['bulkname'] = t('Bulk update node paths');
      $settings['bulkdescr'] =
        t('Generate aliases for all existing nodes which do not already have aliases. Note: Bulk Update may not complete on large or slow sites. See the README.txt for more information.');
      $patterns = token_get_list('node');
      foreach ($patterns['node'] as $pattern => $description) {
        $settings['placeholders'][t('['. $pattern .']')] = t($description);
      }
      $nodetypes = node_get_types();
      foreach ($nodetypes as $ntype => $nodetype) {
        $fieldlabel = t('Pattern for all '. $nodetype->name .' paths');
        $settings['patternitems'][$ntype] = $fieldlabel;
      }
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_nodeapi().
 */
function pathauto_nodeapi(&$node, $op, $teaser, $page) {
  if (module_exists('path')) {
    switch ($op) {
      case 'insert':
      case 'update':
        $placeholders = pathauto_get_placeholders('node', $node);
        $src = "node/$node->nid";
        $alias = pathauto_create_alias('node', $op, $placeholders, $src, $node->type);
        break;
      default:
        break;
    }
  }
}

/**
 * Implementation of hook_form_alter().
 * 
 * A helpful hint to the users with access to alias creation (pathauto
 * will be invisible to other users). Inserted into the path module's
 * fieldset in the node form
 */
function pathauto_form_alter($formid, &$form) {
  
  
  if (isset($form['#attributes']['id']) && ($form['#attributes']['id'] == 'node-form')) {
    if (user_access('create url aliases')) {
      $output .= t('An alias will be automatically generated from the title and other node attributes, in addition to any alias manually provided above.');
      if (user_access('administer pathauto')) {
        $output .= t(' To control the format of the generated aliases, see the <a href="admin/settings/pathauto">pathauto settings</a>.');
      }
    }
    if ($output) {
      $form['path']['pathauto'] = array('#type' => 'item',
        '#description' => $output);
    }
  }
}


/**
 * Generate aliases for all nodes without aliases. 
 */
function node_pathauto_bulkupdate() {
  $query = "SELECT nid, type, title, uid, created, src, dst, vid FROM {node} LEFT JOIN {url_alias} ON CONCAT('node/', nid) = src WHERE src IS NULL";
  $result = db_query($query);
  $node_ref = db_fetch_object($result);

  $count = 0;
  $placeholders = array();
  while ($node_ref) {
    $node = node_load($node_ref->nid, NULL, TRUE);
    $node->src = $node_ref->src;
    $node->dst = $node_ref->dst;
    if (module_exists('taxonomy')) {
        // Must populate the terms for the node here for the category
        // placeholders to work
        $node->taxonomy = array_keys(taxonomy_node_get_terms($node->nid));
    }
    $placeholders = pathauto_get_placeholders('node', $node);
    $src = "node/$node->nid";
    if ($alias = pathauto_create_alias('node', 'bulkupdate', $placeholders, $src, $node->type)) {
      $count++;
    }
    $node_ref = db_fetch_object($result);
  }

  drupal_set_message(format_plural($count,
    "Bulk update of nodes completed, one alias generated.",
    "Bulk update of nodes completed, @count aliases generated."));
}
