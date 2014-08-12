package main

import (
	"os"
	"os/exec"

	"github.com/ivotron/kin"
	"github.com/spf13/cobra"
)

type KinError struct {
	Msg string
}

func (e KinError) Error() string {
	return "kin: " + e.Msg
}

func NewRepository(conf kin.Options) (repo kin.Repository, err error) {
	switch conf.Coordinator {

	case kin.SingleClient:
		if repo, err := kin.NewSingleClientCoordinator(conf); err != nil {
			return nil, err
		} else {
			return repo, nil
		}

	default:
		return nil, KinError{"unknown coordination type " + string(conf.Coordinator)}
	}
}

func main() {
	var cmd *exec.Cmd

	// check if we can execute git
	// {
	cmd = exec.Command("git", "version")

	if _, err := cmd.Output(); err != nil {
		println("Unable to execute 'git': " + err.Error())
		return
	}
	// }

	var cmdStatus = &cobra.Command{
		Use:   "status",
		Short: "Show status of kin project",
		Long: `shows changes of submodules w.r.t. the commit recorded in kin's index or the 
			HEAD (similar to 'git status')`,
		Run: func(cobraCmd *cobra.Command, args []string) {
			cmd = exec.Command("git", "status")
			cmd.Stdout = os.Stdout
			cmd.Stderr = os.Stderr
			cmd.Run()
		},
	}

	var cmdSubmodule = &cobra.Command{
		Use:   "submodule <command>",
		Short: "Initialize, update or inspect submodules",
		Long:  "",
		Run: func(cmd *cobra.Command, args []string) {
		},
	}

	var repoProtocol string
	var repoBranch string
	var cmdSubmoduleAdd = &cobra.Command{
		Use:   "add [-b <branch>] [-p <protocol>] <repository> [<path>]",
		Short: "Add the given repository as a submodule",
		Long:  "",
		Run: func(cmd *cobra.Command, args []string) {

			// 1. Check if config file is accessible. Create it if doesn't exist
			if _, err := os.Stat(".kinsubmodules"); err != nil {
				if os.IsNotExist(err) {
					if _, err := os.Create(".kinsubmodules"); err != nil {
						println("Error: Can't create '.kinsubmodules' file " + err.Error())
						return
					}
				} else {
					println("Error: Can't access '.kinsubmodules' file " + err.Error())
					return
				}
			}

			// 2. Create config for given submodule

			// 3. try to add it to configuration

			// 4. invoke corresponding plugin

			// 5. save .kinmodules
		},
	}

	cmdSubmoduleAdd.Flags().StringVarP(&repoProtocol, "protocol", "p", "", "repository protocol to use (available: git, docker, kin)")
	cmdSubmoduleAdd.Flags().StringVarP(&repoBranch, "branch", "b", "", "branch to point to")

	var cmdSubmoduleStatus = &cobra.Command{
		Use:   "status <path>",
		Short: "Show the status of the given submodule",
		Long: `shows if the SHA1 of the submodule differs with the one from the containing 
			repo.`,
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdSubmoduleUpdate = &cobra.Command{
		Use:   "update <path>",
		Short: "Updates the given submodule",
		Long: `updates the submodules so that the submodule is "remote updated" (i.e. the 
				equilvalent to 'cd module; git remote update' or 'cd module; get fetch').`,
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdFetch = &cobra.Command{
		Use:   "fetch <repository>",
		Short: "Download updates from another repository",
		Long: `Fetches named heads or tags from one or more repositories, along with the 
			objects necessary to complete them. This is equivalent to 'git remote update <repo>'`,
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdAdd = &cobra.Command{
		Use:   "add <pathspec>",
		Short: "Add submodules changes to the index",
		Long: `Updates the index using the commit ID found in the given submodule, to prepare the content staged for the next commit. It can be thought as 
				a way of "forwarding" the ID of the submodule w.r.t. pointed branch in its repository.`,
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdCommit = &cobra.Command{
		Use:   "commit",
		Short: "Record changes to the kin repository",
		Long: `Stores the contents of the index in a new commit along with a log message from 
			the user describing the changes.`,
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdReset = &cobra.Command{
		Use:   "reset <pathspec>",
		Short: "Discard changes",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdRemove = &cobra.Command{
		Use:   "rm <pathspec>",
		Short: "Remove a path",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdBranch = &cobra.Command{
		Use:   "branch <branch-name>",
		Short: "List, create, or delete branches",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdMerge = &cobra.Command{
		Use:   "merge <branch-name>",
		Short: "Join two or more development histories together",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var cmdRebase = &cobra.Command{
		Use:   "rebase <branch-name>",
		Short: "Forward-port local commits to the updated upstream head",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
			return
		},
	}

	var rootCmd = &cobra.Command{Use: "kin"}

	rootCmd.AddCommand(cmdStatus, cmdSubmodule, cmdAdd, cmdCommit, cmdReset, cmdRemove,
		cmdBranch, cmdMerge, cmdRebase, cmdFetch)

	cmdSubmodule.AddCommand(cmdSubmoduleAdd, cmdSubmoduleStatus, cmdSubmoduleUpdate)

	rootCmd.Execute()
}
