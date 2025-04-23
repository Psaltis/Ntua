package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class AddCategoryController {

    @FXML
    private TextField categoryTextField;

    @FXML
    private void handleAddCategory() {
        String newCategory = categoryTextField.getText().trim();
        if (!newCategory.isEmpty()) {
            try {
                CategoryManager.addCategory(newCategory);
                System.out.println("Category added: " + newCategory);
                handleClose();
            } catch (IllegalArgumentException e) {
                System.err.println("Error: " + e.getMessage());
            }
        } else {
            System.err.println("Error: Category name cannot be empty.");
        }
    }
    @FXML
    private void handleClose() {
        Stage stage = (Stage) categoryTextField.getScene().getWindow();
        stage.close();
    }
}