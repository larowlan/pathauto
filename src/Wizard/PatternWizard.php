<?php
/**
 * @file
 * Contains \Drupal\pathauto\Wizard\PatternWizard.
 */

namespace Drupal\pathauto\Wizard;


use Drupal\ctools\Wizard\EntityFormWizardBase;
use Drupal\pathauto\Form\AddContext;
use Drupal\pathauto\Form\ConfigurePatternForm;
use Drupal\pathauto\Form\PathautoPatternForm;
use Drupal\pathauto\Form\SelectionCriteriaForm;

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
        'form' => PathautoPatternForm::class,
      ],
      'contexts' => [
        'title' => $this->t('Add contexts'),
        'form' => AddContext::class,
      ],
      'selection_criteria' => [
        'title' => $this->t('Selection criteria'),
        'form' => SelectionCriteriaForm::class,
      ],
      'pattern' => [
        'title' => $this->t('Configure pattern'),
        'form' => ConfigurePatternForm::class,
      ],
    ];
  }

}