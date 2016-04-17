package molecule;

import java.util.Random;
import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.*;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import javafx.scene.paint.PhongMaterial;
import javafx.scene.shape.Box;
import javafx.scene.shape.Cylinder;
import javafx.scene.shape.Sphere;
import javafx.scene.transform.Rotate;
import javafx.event.EventHandler;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.geometry.Point3D;
import javafx.scene.shape.DrawMode;


/**
 *
 * @author Anton Mitrokhin
 */
public class Molecule extends Application {

    final Group root = new Group();
    final Xform axisGroup = new Xform();
    final Xform moleculeGroup = new Xform();
    final Xform world = new Xform();
    final PerspectiveCamera camera = new PerspectiveCamera(true);
    final Xform cameraXform = new Xform();
    final Xform cameraXform2 = new Xform();
    final Xform cameraXform3 = new Xform();
    Stage primaryStage = new Stage();
    
    private static final int SPHERE_COUNT = 100;
    private final Sphere[] nodes  = new Sphere[SPHERE_COUNT];
    private final Point3D[] poses = new Point3D[SPHERE_COUNT];
    private final Point3D[] vels  = new Point3D[SPHERE_COUNT];
    private double r [] = new double [SPHERE_COUNT];
    private double m [] = new double [SPHERE_COUNT];
    private double q [] = new double [SPHERE_COUNT];
    private boolean is_colliding [] = new boolean [SPHERE_COUNT];

    
    private static final double WORLD_SIZE = 150;
    private static double cam_speed = 0;
    
    private final Random random = new Random();

    private static final double CAMERA_INITIAL_DISTANCE = -450;
    private static final double CAMERA_INITIAL_X_ANGLE = 0.0;
    private static final double CAMERA_INITIAL_Y_ANGLE = 320.0;
    private static final double CAMERA_NEAR_CLIP = 0.1;
    private static final double CAMERA_FAR_CLIP = 10000.0;
    private static final double MOUSE_SPEED = 0.1;
    private static final double ROTATION_SPEED = 2.0;
    
    double mousePosX;
    double mousePosY;
    double mouseOldX;
    double mouseOldY;
    double mouseDeltaX;
    double mouseDeltaY;
    
    private void buildCamera() {
        System.out.println("buildCamera()");
        root.getChildren().add(cameraXform);
        cameraXform.getChildren().add(cameraXform2);
        cameraXform2.getChildren().add(cameraXform3);
        cameraXform3.getChildren().add(camera);
        cameraXform3.setRotateZ(180.0);

        camera.setNearClip(CAMERA_NEAR_CLIP);
        camera.setFarClip(CAMERA_FAR_CLIP);
        camera.setTranslateZ(CAMERA_INITIAL_DISTANCE);
        cameraXform.ry.setAngle(CAMERA_INITIAL_Y_ANGLE);
        cameraXform.rx.setAngle(CAMERA_INITIAL_X_ANGLE);
    }

    private void buildBox() {
        final PhongMaterial redMaterial = new PhongMaterial();
        redMaterial.setDiffuseColor(Color.DARKRED);
        redMaterial.setSpecularColor(Color.RED);

        final PhongMaterial greenMaterial = new PhongMaterial();
        greenMaterial.setDiffuseColor(Color.DARKGREEN);
        greenMaterial.setSpecularColor(Color.GREEN);

        final PhongMaterial blueMaterial = new PhongMaterial();
        blueMaterial.setDiffuseColor(Color.DARKBLUE);
        blueMaterial.setSpecularColor(Color.BLUE);

        double thickness = 0.2;
        final Box bBox0 = new Box(WORLD_SIZE, thickness, thickness);
        final Box bBox1 = new Box(WORLD_SIZE, thickness, thickness);
        final Box bBox2 = new Box(WORLD_SIZE, thickness, thickness);
        final Box bBox3 = new Box(WORLD_SIZE, thickness, thickness);
        bBox0.setTranslateY(WORLD_SIZE/2);
        bBox0.setTranslateZ(WORLD_SIZE/2);
        bBox1.setTranslateY(WORLD_SIZE/2);
        bBox1.setTranslateZ(-WORLD_SIZE/2);
        bBox2.setTranslateY(-WORLD_SIZE/2);
        bBox2.setTranslateZ(WORLD_SIZE/2);
        bBox3.setTranslateY(-WORLD_SIZE/2);
        bBox3.setTranslateZ(-WORLD_SIZE/2);
        
        final Box bBox4 = new Box(thickness, WORLD_SIZE, thickness);
        final Box bBox5 = new Box(thickness, WORLD_SIZE, thickness);
        final Box bBox6 = new Box(thickness, WORLD_SIZE, thickness);
        final Box bBox7 = new Box(thickness, WORLD_SIZE, thickness);
        bBox4.setTranslateX(WORLD_SIZE/2);
        bBox4.setTranslateZ(WORLD_SIZE/2);
        bBox5.setTranslateX(WORLD_SIZE/2);
        bBox5.setTranslateZ(-WORLD_SIZE/2);
        bBox6.setTranslateX(-WORLD_SIZE/2);
        bBox6.setTranslateZ(WORLD_SIZE/2);
        bBox7.setTranslateX(-WORLD_SIZE/2);
        bBox7.setTranslateZ(-WORLD_SIZE/2);
        
        final Box bBox8 = new Box(thickness, thickness, WORLD_SIZE);
        final Box bBox9 = new Box(thickness, thickness, WORLD_SIZE);
        final Box bBox10 = new Box(thickness, thickness, WORLD_SIZE);
        final Box bBox11 = new Box(thickness, thickness, WORLD_SIZE);
        bBox8.setTranslateX(WORLD_SIZE/2);
        bBox8.setTranslateY(WORLD_SIZE/2);
        bBox9.setTranslateX(WORLD_SIZE/2);
        bBox9.setTranslateY(-WORLD_SIZE/2);
        bBox10.setTranslateX(-WORLD_SIZE/2);
        bBox10.setTranslateY(WORLD_SIZE/2);
        bBox11.setTranslateX(-WORLD_SIZE/2);
        bBox11.setTranslateY(-WORLD_SIZE/2);

        axisGroup.getChildren().addAll(bBox0, bBox1, bBox2, bBox3, bBox4, bBox5, 
                                       bBox6, bBox7, bBox8, bBox9, bBox10, bBox11);
        world.getChildren().addAll(axisGroup);
    }
    
    
    private void CollisionWithBox(int i) {
        if(((poses[i].getX() - r[i] < -WORLD_SIZE/2) && (vels[i].getX() < 0)) || 
           ((poses[i].getX() + r[i] > WORLD_SIZE/2) && (vels[i].getX() > 0))) {
            vels[i] = new Point3D(-vels[i].getX()*0.7, vels[i].getY(), vels[i].getZ());
        }
        if(((poses[i].getY() - r[i] < -WORLD_SIZE/2) && (vels[i].getY() < 0)) || 
           ((poses[i].getY() + r[i] > WORLD_SIZE/2) && (vels[i].getY() > 0))) {
            vels[i] = new Point3D(vels[i].getX(), -vels[i].getY()*0.7, vels[i].getZ());
        }           
        if(((poses[i].getZ() - r[i] < -WORLD_SIZE/2) && (vels[i].getZ() < 0)) || 
           ((poses[i].getZ() + r[i] > WORLD_SIZE/2) && (vels[i].getZ() > 0))) {
            vels[i] = new Point3D(vels[i].getX(), vels[i].getY(), -vels[i].getZ()*0.7);
        }         
    }
    
    private void CollisionWithNodes(int i) {
        for (int j=0; j<SPHERE_COUNT; j++) {
            if (i == j) continue;
            if ((poses[i].distance(poses[j]) < r[i] + r[j]) &&
               (vels[j].subtract(vels[i]).dotProduct(poses[j].subtract(poses[i]))) < 0) {
                
                Point3D new_i = vels[i].multiply(m[i] - m[j]).add(vels[j].multiply(2*m[j])).multiply(1/(m[i]+m[j]));
                Point3D new_j = vels[j].multiply(m[j] - m[i]).add(vels[i].multiply(2*m[i])).multiply(1/(m[i]+m[j]));
                vels[i] = new_i.multiply(0.7);
                vels[j] = new_j.multiply(0.7);
                //System.out.println(i + ": " + vels[i] + "   " + vels[j]);
            }
        }
    }
    
    private void Field(int i, double dt) {
        for (int j=0; j<SPHERE_COUNT; j++) {
            if (i == j) continue;
            Point3D r21 = poses[i].subtract(poses[j]);
            double d = r21.magnitude();
            if (d < r[i] + r[j]) continue;
                
            Point3D force = r21.normalize().multiply(1000*q[i]*q[j]/(d*d));
            
            Point3D acl1 = force.multiply(1/m[i]);
            Point3D acl2 = force.multiply(1/m[j]);
            if(acl1.magnitude() > 800) acl1 = acl1.multiply(800/acl1.magnitude());
            if(acl2.magnitude() > 800) acl2 = acl1.multiply(800/acl2.magnitude());

            Point3D next_x1 = poses[i].add(vels[i].multiply(dt));
            Point3D next_x2 = poses[j].add(vels[j].multiply(dt));

            if (next_x1.subtract(next_x2).magnitude() > r[i] + r[j])
                vels[i] = vels[i].add(acl1.multiply(dt));
        }

        //System.out.println(i + ": m = " + m[i] + "   " + force);

    }
        
    private void handleMouse(Scene scene, final Node root) {
        scene.setOnMousePressed(new EventHandler<MouseEvent>() {
            @Override public void handle(MouseEvent me) {
                mousePosX = me.getSceneX();
                mousePosY = me.getSceneY();
                mouseOldX = me.getSceneX();
                mouseOldY = me.getSceneY();
            }
        });
        scene.setOnMouseDragged(new EventHandler<MouseEvent>() {
            @Override public void handle(MouseEvent me) {
                mouseOldX = mousePosX;
                mouseOldY = mousePosY;
                mousePosX = me.getSceneX();
                mousePosY = me.getSceneY();
                mouseDeltaX = (mousePosX - mouseOldX);
                mouseDeltaY = (mousePosY - mouseOldY); 
                cam_speed = mouseDeltaX;
                
                
                if (me.isPrimaryButtonDown()) {
                    cameraXform.ry.setAngle(cameraXform.ry.getAngle() - mouseDeltaX*MOUSE_SPEED*ROTATION_SPEED);  
                    //cam_speed = 0;
                }
                else if (me.isSecondaryButtonDown()) {
                    cameraXform.ry.setAngle(cameraXform.ry.getAngle() - mouseDeltaX*MOUSE_SPEED*ROTATION_SPEED);  
                    cameraXform.rx.setAngle(cameraXform.rx.getAngle() + mouseDeltaY*MOUSE_SPEED*ROTATION_SPEED);
                    //cam_speed = 0;
                }                
            }
        });
    }
    
    private void handleKeyboard(Scene scene, final Node root) {
        scene.setOnKeyPressed(new EventHandler<KeyEvent>() {
            @Override
            public void handle(KeyEvent event) {
                switch (event.getCode()) {
                    case Z:
                        cam_speed = 0;
                        cameraXform2.t.setX(0.0);
                        cameraXform2.t.setY(0.0);
                        camera.setTranslateZ(CAMERA_INITIAL_DISTANCE);
                        cameraXform.ry.setAngle(CAMERA_INITIAL_Y_ANGLE);
                        cameraXform.rx.setAngle(CAMERA_INITIAL_X_ANGLE);
                        break;
                    case X:
                        axisGroup.setVisible(!axisGroup.isVisible());
                        break;
                    case V:
                        moleculeGroup.setVisible(!moleculeGroup.isVisible());
                        break;
                }
            }
        });
    }
    
    private void buildMolecule() {
        for (int i=0; i<SPHERE_COUNT; i++) {
            r[i] = 2;//random.nextInt(10) + 1;
            m[i] = r[i];
            q[i] = 1;//(random.nextDouble()-0.5)*100;
            
            
            poses[i] = new Point3D(random.nextDouble()*WORLD_SIZE - WORLD_SIZE/2, 
                                   random.nextDouble()*WORLD_SIZE - WORLD_SIZE/2, 
                                   random.nextDouble()*WORLD_SIZE - WORLD_SIZE/2);
            
            vels[i]  = new Point3D(random.nextInt(100), random.nextInt(100), random.nextInt(100));
            //vels[i]  = new Point3D(0, 0, 0);
        }

    }
    
    private void spawnNodes() {
        final PhongMaterial redMaterial = new PhongMaterial();
        redMaterial.setDiffuseColor(Color.DARKRED);
        redMaterial.setSpecularColor(Color.RED);

        final PhongMaterial blueMaterial = new PhongMaterial();
        blueMaterial.setDiffuseColor(Color.DARKBLUE);
        blueMaterial.setSpecularColor(Color.BLUE);
        
        for (int i=0; i<SPHERE_COUNT; i++) {
            nodes[i] = new Sphere(r[i]);
            if (q[i] > 0)
                nodes[i].setMaterial(redMaterial);
            if (q[i] < 0)
                nodes[i].setMaterial(blueMaterial);
        }
        world.getChildren().addAll(new Group(nodes));
    }

    @Override
    public void start(final Stage primaryStage_) {
        primaryStage = primaryStage_;
        
        root.getChildren().add(world);
        root.setDepthTest(DepthTest.ENABLE);

        // buildScene();
        buildCamera();
        buildBox();
        buildMolecule();
        
        m[0] = 9000;
        r[0] = 10;
        poses[0] = new Point3D(0, 0, 0);
        vels[0]  = new Point3D(0, 0, 0);
        q[0] = -40;
        
        spawnNodes();
        final Scene scene = new Scene(root, 1920, 1080, true);
        scene.setFill(Color.GREY);
        handleKeyboard(scene, world);
        handleMouse(scene, world);

        primaryStage.setTitle("Molecule");
        primaryStage.setScene(scene);
        primaryStage.show();

        scene.setCamera(camera);
        
        AnimationTimer timer = new MyTimer();
        timer.start();
    }
    
    private class MyTimer extends AnimationTimer {
        long old_now = 0;
        
        @Override
        public void handle(long now) {
            doHandle(now);
        }

        private void doHandle(long now) {
            long dt = now - old_now;               
            //System.out.println((float)dt/1000000000.0);

            if (dt > 1000000000/10) {
                dt = 0;
            }

            
            old_now = now;
            final double width = 0.5 * primaryStage.getWidth();
            final double height = 0.5 * primaryStage.getHeight();

            for (int i=0; i<SPHERE_COUNT; i++) {
                Field(i, (float)dt/1000000000.0);
                CollisionWithBox(i);
                CollisionWithNodes(i);

                poses[i] = poses[i].add(vels[i].multiply((float)dt/1000000000.0));
                
                nodes[i].setTranslateX(poses[i].getX());
                nodes[i].setTranslateY(poses[i].getY());
                nodes[i].setTranslateZ(poses[i].getZ());
            }
            
            cameraXform.ry.setAngle(cameraXform.ry.getAngle() - cam_speed*MOUSE_SPEED*ROTATION_SPEED);
            cam_speed = cam_speed/1.2;
        }
    }
    
    

    /**
     * The main() method is ignored in correctly deployed JavaFX application.
     * main() serves only as fallback in case the application can not be
     * launched through deployment artifacts, e.g., in IDEs with limited FX
     * support. NetBeans ignores main().
     *
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }

}