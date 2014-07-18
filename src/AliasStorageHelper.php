<?php

/**
 * @file
 * Contains Drupal\pathauto\AliasSchemaHelper
 */

namespace Drupal\pathauto;

/**
 * Provides helper methods for accessing alias storage.
 */
class AliasStorageHelper implements AliasStorageHelperInterface {

  /**
   * Alias schema max length.
   *
   * @var int
   */
  protected $aliasSchemaMaxLength;

  /**
   * {@inheritdoc}
   */
  public function getAliasSchemaMaxLength() {
    if (!isset($this->aliasSchemaMaxLength)) {
      $schema = drupal_get_schema('url_alias');
      $this->aliasSchemaMaxLength = $schema['fields']['alias']['length'];
    }
    return $this->aliasSchemaMaxLength;
  }

}
