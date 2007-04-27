<?php
// $Id$

/*
 * Implementation of hook_pathauto_node()
 */

function i18n_pathauto_node($op, $node = NULL) {
  switch ($op) {
    case 'placeholders':
      $placeholders = array();
      $placeholders[t('[lang]')] = t('');
      return $placeholders;
    case 'values':
      $results = array();
      $results[t('[lang]')] = 
      return $results;
    default:
      break;
  }
}


/*
 * Implementation of hook_token_list()
 */
function pathauto_node_i18n_token_list($type = 'all') {
  if ($type == 'node' || $type == 'all'):
    $tokens['node']['lang']   = t("Language code of the document");
    return $tokens;
  }
}

/*
 * Implementation of hook_token_values()
 */
 
function pathauto_node_i18n_token_values($type, $object = NULL) {
  if ($type == 'node'):
    $tokens['lang']      = i18n_node_get_lang($node->nid, i18n_get_lang());
    return $tokens;
  }
}

