#include <ros/ros.h>
#include <control_msgs/FollowJointTrajectoryAction.h>
#include <actionlib/client/simple_action_client.h>
#include <sensor_msgs/JointState.h>
#include <iostream>

//vairabile globale per la lettura della configurazione iniziale

typedef actionlib::SimpleActionClient<control_msgs::FollowJointTrajectoryAction> Client;

class MyNode
{
  private:
      Client ac;

  public: 
    sensor_msgs::JointState q0; 
    int indice;
    ros::NodeHandle n;
    ros::Timer timer_;
    std::vector<std::string> joint_names_ = {"panda_joint1",
                                             "panda_joint2",
                                             "panda_joint3",
                                             "panda_joint4",
                                             "panda_joint5",
                                             "panda_joint6",
                                             "panda_joint7"};
   

    MyNode() : ac("/position_joint_trajectory_controller/follow_joint_trajectory", true)
    {
        indice=0;
        ros::Time start=ros::Time::now();
        auto msg=ros::topic::waitForMessage<sensor_msgs::JointState>("/joint_states",ros::Duration(1.0));
        for (size_t i = 0; i < msg->name.size(); ++i) 
        {
            ROS_INFO("Nome: %s, Posizione: %f", msg->name[i].c_str(), msg->position[i]);
        }
        q0=*msg;

        ROS_INFO("Waiting for action server to start.");
        ac.waitForServer();
        ROS_INFO("Action server started, sending goal."); 
        timer_ =n.createTimer(ros::Duration(1,0), &MyNode::timerCallback,this); //duration(seconds,nanoseconds)
    }

    void timerCallback(const ros::TimerEvent&)
    {
        // send a goal to the action
        control_msgs::FollowJointTrajectoryActionGoal goal;
    
        goal.goal.trajectory.joint_names=joint_names_;

      
        //goal.header.seq=1;
        ros::Duration delta(0,0);
        goal.header.stamp=ros::Time::now()+delta;
        //goal.header.frame_id="ci vuole una stringa";
        goal.goal_id.stamp=ros::Time::now()+delta;
        //goal.goal_id.id="stringa";
        //goal.goal.trajectory.header.seq=1;
        goal.goal.trajectory.header.stamp=ros::Time(0);
        //goal.goal.trajectory.header.frame_id="stringa";
        goal.goal.trajectory.joint_names=joint_names_;
        goal.goal.trajectory.points.resize(1);
        //goal.goal.trajectory.points[0].positions=q0.position;
        goal.goal.trajectory.points[0].positions.resize(7);
        goal.goal.trajectory.points[0].positions = q0.position;

        if(indice<=20) //
         {
            goal.goal.trajectory.points[0].positions[6]+=0.035; //incremento di 2 gradi
            q0.position[6]+=0.035;
         }

         else if(indice>20 && indice <40) //
         {
            goal.goal.trajectory.points[0].positions[6]-=0.035; //incremento di 2 gradi
            q0.position[6]-=0.035;
         }

         else
         {
           indice=0;
         }

        indice++;
        //goal.goal.trajectory.points.velocities=vettore di veelocità;
        //goal.goal.trajectory.points.accelerations=vettore di accelerazioni
        //goal.goal.trajectory.points.effort=vettore di forze;

        ros::Duration iniziale(0,0);
        ros::Duration finale(0.95,0); //il goal deve essere raggiunto in 15 ms
        goal.goal.trajectory.points[0].time_from_start=finale;

        //goal.goal.path_tolerance.resize(1);
        //goal.goal.path_tolerance[0].name="tolleranza1";
        //goal.goal.trajectory.path_tolerance.position=1.0;
        //goal.goal.trajectory.path_tolerance.veelocity=1.0;
        //goal.goal.trajectory.path_tolerance.acceeleration=1.0;

        goal.goal.goal_tolerance.resize(1);
        //goal.goal.goal_tolerance[0].name="tolleranza2";
        //goal.goal.trajectory.goal_tolerance.position=1.
        //goal.goal.trajectory.goal_tolerance.velocity=1.0;
        //goal.goal.trajectory.goal_tolerance.acceleration=1.0;
        goal.goal.goal_time_tolerance=ros::Duration(0, 500000000);

        ROS_INFO("Sono nella callback"); 

        //invia la richiesta all'action_server
        ac.sendGoal(goal.goal);
        

        //wait for the action to return
        bool finished_before_timeout = ac.waitForResult(ros::Duration(30.0));

        if (finished_before_timeout)
        {
          actionlib::SimpleClientGoalState state = ac.getState();
          ROS_INFO("Action finished: %s",state.toString().c_str());
        }
        else
        {
          ROS_INFO("Action did not finish before the time out.");
          ac.cancelGoal();
        }
    }


};

int main (int argc, char **argv)
{
  ros::init(argc, argv, "prova_movimento3");
  MyNode my_node;
  //my_node.doStuff(10);
  ros::spin();
  return 0;
}

