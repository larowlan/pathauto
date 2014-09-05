<?php

/**
 * @file
 * Contains Drupal\pathauto\Plugin\AliasType\ContentEntityAliasType
 */

namespace Drupal\pathauto\Plugin\AliasType;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Language\LanguageInterface;

/**
 * A pathauto alias type plugin for content entities.
 *
 * @AliasType(
 *   id = "content_entity_alias_type",
 *   label = @Translation("Pathauto alias for nodes."),
 * )
 */
class ContentEntityAliasType extends AliasTypeBase {

  /**
   * {@inheritdoc}
   */
  public function getLabel() {
    return $this->t('Content paths');
  }

  /**
   * {@inheritdoc}
   */
  public function getPatternDescription() {
    $this->t('Default path pattern (applies to all content types with blank patterns below)');
  }

  /**
   * {@inheritdoc}
   */
  public function getTokenType() {
    return 'node';
  }

  /**
   * {@inheritdoc}
   */
  public function getPatterns() {
    $patterns = [];
    $languages = $this->getLanguages();
    foreach ($this->getNodeTypeNames() as $node_type => $node_name) {
      if (count($languages) && $this->isContentTranslationEnabled($node_type)) {
        $patterns[$node_type] = $this->t('Default path pattern for @node_type (applies to all @node_type content types with blank patterns below)', array('@node_type' => $node_name));
        foreach ($languages as $lang_code => $lang_name) {
          $patterns[$node_type . '_' . $lang_code] = $this->t('Pattern for all @language @node_type paths', array('@node_type' => $node_name, '@language' => $lang_name));
        }
      }
      else {
        $patterns[$node_type] = $this->t('Pattern for all @node_type paths', array('@node_type' => $node_name));
      }
    }
    return $patterns;
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    // tODO: Implement submitConfigurationForm() method.
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return array('patternitems' => array('content/[node:title]')) + parent::defaultConfiguration();
  }

  /**
   * Wraps node_type_get_names().
   *
   * @return array
   *   An array of node type names, keyed by type.
   */
  protected function getNodeTypeNames() {
    return node_type_get_names();
  }

  /**
   * Gets a list of languages if locale module is enabled.
   *
   * @return \Drupal\Core\Language\LanguageInterface[]
   *   An array of languages. An empty array if locale module is disabled.
   */
  protected function getLanguages() {
    $languages = array();
    if (\Drupal::moduleHandler()->moduleExists('locale')) {
      $languages = array(LanguageInterface::LANGCODE_NOT_SPECIFIED => $this->t('language neutral')) + \Drupal::languageManager()->getLanguages('name');
    }
    return $languages;
  }

  /**
   * Wraps content_translation_enabled().
   *
   * @param string $node_type
   *   the node type.
   *
   * @return bool
   *   tRUE if content translation is enabled for the content type.
   */
  protected function isContentTranslationEnabled($node_type) {
    return \Drupal::moduleHandler()->moduleExists('content_translation') && content_translation_enabled('node', $node_type);
  }

}
