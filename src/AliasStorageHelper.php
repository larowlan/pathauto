<?php

/**
 * @file
 * Contains Drupal\pathauto\AliasSchemaHelper
 */

namespace Drupal\pathauto;

/**
 * Provides helper methods for accessing alias storage.
 */
class AliasStorageHelper {

  /**
   * Alias schema max length.
   *
   * @var int
   */
  protected $aliasSchemaMaxLength;

  /**
   * Fetch the maximum length of the {url_alias}.alias field from the schema.
   *
   * @return int
   *   An integer of the maximum URL alias length allowed by the database.
   */
  public function getAliasSchemaMaxLength() {
    if (!isset($this->aliasSchemaMaxLength)) {
      $schema = drupal_get_schema('url_alias');
      $this->aliasSchemaMaxLength = $schema['fields']['alias']['length'];
    }
    return $this->aliasSchemaMaxLength;
  }

}
