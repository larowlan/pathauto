<?php
/**
 * @file
 * Contains \Drupal\pathauto\Wizard\PatternWizard.
 */

namespace Drupal\pathauto\Wizard;


use Drupal\ctools\Wizard\EntityFormWizardBase;

/**
 * Custom form wizard for pathauto pattern configuration.
 */
class PatternWizard extends EntityFormWizardBase {

  /**
   * {@inheritdoc}
   */
  public function getWizardLabel() {
    return $this->t('Pattern');
  }

  /**
   * {@inheritdoc}
   */
  public function getMachineLabel() {
    return $this->t('Identifier');
  }

  /**
   * {@inheritdoc}
   */
  public function getEntityType() {
    return 'pathauto_pattern';
  }

  /**
   * {@inheritdoc}
   */
  public function exists() {
    return 'Drupal\pathauto\Entity\PathautoPattern::load';
  }

  /**
   * {@inheritdoc}
   */
  public function getOperations() {
    return [
      'general' => [
        'title' => $this->t('General information'),
        'form' => '\Drupal\pathauto\Form\PathautoPatternForm'
      ],
      'selection_criteria' => [
        'title' => $this->t('Selection criteria'),
        'form' => '\Drupal\pathauto\Form\SelectionCriteriaForm'
      ],
      'pattern' => [
        'title' => $this->t('Configure pattern'),
        'form' => '\Drupal\pathauto\Form\ConfigurePatternForm'
      ]
    ];
  }

}