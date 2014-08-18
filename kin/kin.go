package main

import (
	"os"
	"os/exec"

	"github.com/ivotron/kin"
	"github.com/spf13/cobra"
)

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

	var cmdInit = &cobra.Command{
		Use:   "init [--odb <backend>] [--mdb <backend>]",
		Short: "",
		Long:  ``,
		Run: func(cobraCmd *cobra.Command, args []string) {
			if len(args) > 0 {
				println("Arguments not supported yet")
				return
			}

			if err := kin.Init(); err != nil {
				println(err.Error())
			}
		},
	}

	var cmdCheckout = &cobra.Command{
		Use:   "checkout [<commit>]",
		Short: "",
		Long:  ``,
		Run: func(cobraCmd *cobra.Command, args []string) {
			if len(args) > 1 {
				println("Usage: \n" + cobraCmd.UseLine())
			}
			if err := kin.Checkout(args); err != nil {
				println(err.Error())
			}
		},
	}

	var cmdAdd = &cobra.Command{
		Use:   "add <path-spec> [<path-spec> ..]",
		Short: "",
		Long:  ``,
		Run: func(cobraCmd *cobra.Command, args []string) {
			if err := kin.Add(args); err != nil {
				println(err.Error())
			}
		},
	}

	var cmdStatus = &cobra.Command{
		Use:   "status",
		Short: "Show status of kin project",
		Long:  ``,
		Run: func(cobraCmd *cobra.Command, args []string) {
			cmd = exec.Command("git", "status")
			cmd.Stdout = os.Stdout
			cmd.Stderr = os.Stderr
			cmd.Run()
		},
	}

	var cmdCommit = &cobra.Command{
		Use:   "commit",
		Short: "Record changes to the kin repository",
		Long:  ``,
		Run: func(cmd *cobra.Command, args []string) {
			if err := kin.Commit(); err != nil {
				println(err.Error())
			}
		},
	}

	/*
		var cmdReset = &cobra.Command{
			Use:   "reset",
			Short: "Discard changes",
			Long:  ".",
			Run: func(cmd *cobra.Command, args []string) {
				if err := kin.Reset(); err != nil {
					println(err.Error())
				}
			},
		}
	*/

	var cmdRemove = &cobra.Command{
		Use:   "rm <pathspec>",
		Short: "Remove a path",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			if err := kin.Remove(args); err != nil {
				println(err.Error())
			}
		},
	}

	var cmdBranch = &cobra.Command{
		Use:   "branch <branch-name>",
		Short: "List, create, or delete branches",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
		},
	}

	var cmdMerge = &cobra.Command{
		Use:   "merge <branch-name>",
		Short: "Join two or more development histories together",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
		},
	}

	var cmdRebase = &cobra.Command{
		Use:   "rebase <branch-name>",
		Short: "Forward-port local commits to the updated upstream head",
		Long:  ".",
		Run: func(cmd *cobra.Command, args []string) {
			println("Error: Not implemented yet")
		},
	}

	var rootCmd = &cobra.Command{Use: "kin"}

	rootCmd.AddCommand(cmdInit, cmdCheckout, cmdStatus, cmdAdd, cmdCommit, cmdRemove,
		cmdBranch, cmdMerge, cmdRebase)

	rootCmd.Execute()
}
