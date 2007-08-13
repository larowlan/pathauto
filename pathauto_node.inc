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
      $settings['bulkname'] = t('Bulk generate aliases for nodes that are not aliased');
      $settings['bulkdescr'] =
        t('Generate aliases for all existing nodes which do not already have aliases. Note: Bulk Update may not complete on large or slow sites. See the README.txt for more information.');

      $patterns = token_get_list('node');
      $pattern_types = array('node', 'book', 'CCK text', 'CCK node reference', 'CCK number', 'CCK user reference', 'CCK link');
      foreach ($pattern_types as $pattern_type) {
	foreach ($patterns[$pattern_type] as $pattern => $description) {
	  $settings['placeholders'][t('['. $pattern .']')] = t($description);
	}
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
 * Generate aliases for all nodes without aliases. 
 */
function node_pathauto_bulkupdate() {
  $query = "SELECT nid, type, title, uid, created, src, dst, vid FROM {node} LEFT JOIN {url_alias} ON CONCAT('node/', nid) = src WHERE src IS NULL";
  $result = db_query_range($query, 0, variable_get('pathauto_max_bulk_update', 50));

  $count = 0;
  $placeholders = array();
  while ($node_ref = db_fetch_object($result)) {
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
  }

  drupal_set_message(format_plural($count,
    "Bulk generation of nodes completed, one alias generated.",
    "Bulk generation of nodes completed, @count aliases generated."));
}
