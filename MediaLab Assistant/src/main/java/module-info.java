module ntua.medialab.medialabassistant {
    requires javafx.controls;
    requires javafx.fxml;
    requires org.json;

    opens ntua.medialab.medialabassistant to javafx.fxml;
    exports ntua.medialab.medialabassistant;
}