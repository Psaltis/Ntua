package ntua.medialab.medialabassistant;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class Main extends Application {
    @Override
    public void start(Stage primaryStage) {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("MedialabApplication.fxml"));
            Parent root = loader.load();

            primaryStage.setTitle("MediaLab Assistant");
            primaryStage.setScene(new Scene(root, 1000, 900));
            primaryStage.show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    @Override
    public void stop() {
        saveData();
        ReminderManager.cancelAllReminders();
    }
    private void saveData() {
        TaskManager.setTasks();
        CategoryManager.setCategories();
        PriorityManager.setPriorities();
        ReminderManager.setReminders();
    }
    public static void main(String[] args) {
        launch(args);
    }
}
