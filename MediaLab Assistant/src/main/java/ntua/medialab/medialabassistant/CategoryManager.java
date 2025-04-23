package ntua.medialab.medialabassistant;

import org.json.JSONArray;
import org.json.JSONObject;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class CategoryManager {
    private static final String FILE_NAME = "medialab/categories.json";
    private static List<String> categories = null;
    public static void ensureInitialized() {
        if (categories == null || categories.isEmpty()) {
            categories = loadCategories();
            if (categories == null) {
                categories = new ArrayList<>();
            }
        }
    }
    public static void addCategory(String category) {
        if (categories.contains(category)) {
            throw new IllegalArgumentException("Category '" + category + "' already exists.");
        }
        categories.add(category);
    }
    public static void editCategory(String oldCategory, String newCategory) {
        if (!categories.contains(oldCategory)) {
            throw new IllegalArgumentException("Category '" + oldCategory + "' does not exist.");
        }
        int index = categories.indexOf(oldCategory);
        categories.set(index, newCategory);
        TaskManager.updateTasksForRenamedCategory(oldCategory, newCategory);
    }
    public static void removeCategory(String category) {
        if (!categories.contains(category)) {
            throw new IllegalArgumentException("Category '" + category + "' does not exist.");
        }
        TaskManager.removeTasksByCategory(category);
        categories.remove(category);
    }
    public static void setCategories(){
        saveCategories();
    }
    public static List<String> getCategories() {
        if (categories == null || categories.isEmpty()) {
            System.out.println("CategoryManager: No categories loaded!"); // Debug log
        } else {
            System.out.println("CategoryManager: Loaded categories - " + categories);
        }
        return categories;
    }
    private static void saveCategories() {
        JSONArray jsonArray = new JSONArray(categories);
        try (FileWriter file = new FileWriter(FILE_NAME)) {
            file.write(jsonArray.toString(2));
            file.flush();
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to save categories to file: " + FILE_NAME, e);
        }
    }
    private static List<String> loadCategories() {
        if (!Files.exists(Paths.get(FILE_NAME))) {
            return null;
        }
        try {
            String content = new String(Files.readAllBytes(Paths.get(FILE_NAME)));
            JSONArray jsonArray = new JSONArray(content);
            List<String> loadedCategories = new ArrayList<>();
            for (int i = 0; i < jsonArray.length(); i++) {
                loadedCategories.add(jsonArray.getString(i));
            }
            return loadedCategories;
        } catch (IOException e) {
            e.printStackTrace();
            throw new RuntimeException("Failed to load categories from file: " + FILE_NAME, e);
        }
    }
}
