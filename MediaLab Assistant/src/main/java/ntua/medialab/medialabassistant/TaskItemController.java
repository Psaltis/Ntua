package ntua.medialab.medialabassistant;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;

import java.io.IOException;

public class TaskItemController {

    @FXML
    private Label titleLabel;

    @FXML
    private Label categoryLabel;

    @FXML
    private Label priorityLabel;

    @FXML
    private Label deadlineLabel;

    @FXML
    private Label statusLabel;

    @FXML
    private Button view_description_button;

    @FXML
    private Button edittask_button;

    @FXML
    private Button removetask_button;

    private Task task;
    private MedialabApplicationController mainController;

    public void setTask(Task task) {
        this.task = task;
        titleLabel.setText(task.getTitle());
        categoryLabel.setText(task.getCategory());
        priorityLabel.setText(task.getPriority());
        deadlineLabel.setText(task.getDeadline() != null ? task.getDeadline().toString() : "No Deadline");
        statusLabel.setText(task.getStatus().label);
    }
    public void setMainController(MedialabApplicationController mainController) {
        this.mainController = mainController;
    }

    @FXML
    private void handleViewDescription() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("Description.fxml"));
            Parent root = loader.load();

            DescriptionController controller = loader.getController();
            controller.setDescription(task.getDescription());
            Stage stage = new Stage();
            stage.setTitle("Description");
            stage.setScene(new Scene(root));
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    private void handleEditTask() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("EditTask.fxml"));
            Parent root = loader.load();

            EditTaskController controller = loader.getController();
            controller.setTask(task);
            controller.setMainController(mainController);

            Stage stage = new Stage();
            stage.setTitle("Edit Task");
            stage.setScene(new Scene(root));
            stage.show();
        } catch (IOException e) {
            e.printStackTrace();
        }

        mainController.updateReminderList();
    }

    @FXML
    private void handleRemoveTask() {
        TaskManager.removeTask(task);
        if (mainController != null) {
            mainController.updateTaskList();
        }
        System.out.println("Task removed: " + task.getTitle());

        mainController.updateReminderList();
    }
}