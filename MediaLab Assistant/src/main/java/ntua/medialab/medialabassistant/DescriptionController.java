package ntua.medialab.medialabassistant;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

public class DescriptionController {

    private String description;

    @FXML
    private Button closeButton;

    @FXML
    private TextArea descriptionTextArea;

    public void setDescription(String description) {
        this.description = description;
        updateDescriptionTextArea(); // Update the text area immediately after setting the description
    }
    private void updateDescriptionTextArea() {
        if (descriptionTextArea != null && description != null) {
            descriptionTextArea.setText(description);
        }
    }
    @FXML
    private void handleClose() {
        Stage stage = (Stage) descriptionTextArea.getScene().getWindow();
        stage.close();
    }
}
