<?php

/**
 * @file
 * Contains Drupal\pathauto\AliasCleanerInterface
 */
namespace Drupal\pathauto;

/**
 * @todo add class comment.
 */
interface AliasCleanerInterface {

  /**
   * Clean up an URL alias.
   *
   * Performs the following alterations:
   * - Trim duplicate, leading, and trailing back-slashes.
   * - Trim duplicate, leading, and trailing separators.
   * - Shorten to a desired length and logical position based on word boundaries.
   *
   * @param string $alias
   *   A string with the URL alias to clean up.
   *
   * @return string
   *   The cleaned URL alias.
   */
  public function cleanAlias($alias);
}
