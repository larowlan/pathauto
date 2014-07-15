<?php

/**
 * @file
 * Contains \Drupal\pathauto\AliasStorageHelperInterface
 */

namespace Drupal\pathauto;

/**
 * Provides helper methods for accessing alias storage.
 */
interface AliasStorageHelperInterface {

  /**
   * Fetch the maximum length of the {url_alias}.alias field from the schema.
   *
   * @return int
   *   An integer of the maximum URL alias length allowed by the database.
   */
  public function getAliasSchemaMaxLength();

}
