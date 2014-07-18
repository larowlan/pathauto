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

  /**
   * Private function for Pathauto to create an alias.
   *
   * @param array $path
   *   An associative array containing the following keys:
   *   - source: The internal system path.
   *   - alias: The URL alias.
   *   - pid: (optional) Unique path alias identifier.
   *   - language: (optional) The language of the alias.
   * @param array|bool|null $existing_alias
   *   (optional) An associative array of the existing path alias.
   * @param string $op
   *   An optional string with the operation being performed.
   *
   * @return array|bool
   *   The saved path or NULL if the path was not saved.
   */
  public function save(array $path, $existing_alias = NULL, $op = NULL);

}
