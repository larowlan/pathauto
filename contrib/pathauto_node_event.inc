<?php
// $Id$

/*
 * Implementation of hook_token_list()
 */
function pathauto_node_event_token_list($type = 'all') {
  if ($type == 'node' || $type == 'all'):
    $tokens['node']['eventyyyy']   = t("The year the event starts.");
    $tokens['node']['eventmm']     = t("The two-digit month (01-12) the event starts.");
    $tokens['node']['eventmon']    = t("The three-letter month (jan-dec) the event starts.");
    $tokens['node']['eventdd']     = t("The two-digit day of the month (00-31) the event starts.");
    $tokens['node']['eventday']    = t("The three-letter day of the week (sun-sat) the event starts.");
    $tokens['node']['eventweek']   = t("The week number (1-52) of the year the event starts.");
    return $tokens;
  }
}

/*
 * Implementation of hook_token_values()
 */
 
function pathauto_node_event_token_values($type, $object = NULL) {
  if ($type == 'node'):
    $tokens['eventyyyy']      = date('Y', $eventstart);
    $tokens['eventmm']      = date('m', $eventstart);
    $tokens['eventmon']      = pathauto_cleanstring(date('M', $eventstart));
    $tokens['eventdd']      = date('d', $eventstart);
    $tokens['eventday']      = pathauto_cleanstring(date('D', $eventstart));
    $tokens['eventweek']      = date('W', $eventstart);
    return $tokens;
  }
}

