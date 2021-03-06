/*
 * lispd_mapping.h
 *
 * This file is part of LISP Mobile Node Implementation.
 * Send registration messages for each database mapping to
 * configured map-servers.
 *
 * Copyright (C) 2011 Cisco Systems, Inc, 2011. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Please send any bug reports or fixes you make to the email address(es):
 *    LISP-MN developers <devel@lispmob.org>
 *
 * Written or modified by:
 *    Albert Lopez      <alopez@ac.upc.edu>
 */

#ifndef LISPD_MAPPING_H_
#define LISPD_MAPPING_H_

#include "lispd_locator.h"



/****************************************  STRUCTURES **************************************/

/*
 * lispd mapping entry.
 */
typedef struct lispd_mapping_elt_ {
    lisp_addr_t                     eid_prefix;
    uint8_t                         eid_prefix_length;
    int                             iid;
    uint16_t                        locator_count;
    lispd_locators_list             *head_v4_locators_list;
    lispd_locators_list             *head_v6_locators_list;
    uint8_t                         mapping_type;
    void                            *extended_info;
} lispd_mapping_elt;

/*
 * lispd mapping list
 */
typedef struct lispd_mapping_list_ {
    lispd_mapping_elt          *mapping;
    struct lispd_mapping_list_ *next;
}lispd_mapping_list;


/*
 * Used to select the locator to be used for an identifier according to locators' priority and weight.
 *  v4_balancing_locators_vec: If we just have IPv4 RLOCs
 *  v6_balancing_locators_vec: If we just hace IPv6 RLOCs
 *  balancing_locators_vec: If we have IPv4 & IPv6 RLOCs
 *  For each packet, a hash of its tuppla is calculaed. The result of this hash is one position of the array.
 */

typedef struct balancing_locators_vecs_ {
    lispd_locator_elt               **v4_balancing_locators_vec;
    lispd_locator_elt               **v6_balancing_locators_vec;
    lispd_locator_elt               **balancing_locators_vec;
    int v4_locators_vec_length;
    int v6_locators_vec_length;
    int locators_vec_length;
}balancing_locators_vecs;


/*
 * Structure to expand the lispd_mapping_elt used in lispd_map_cache_entry
 */

typedef struct lcl_mapping_extended_info_ {
    balancing_locators_vecs         outgoing_balancing_locators_vecs;
    lispd_locators_list             *head_not_init_locators_list; //List of locators not initialized: interface without ip
    nonces_list                     *map_reg_nonce;
    timer                           *map_reg_timer;
    uint8_t                         to_do_smr;
}lcl_mapping_extended_info;

/*
 * Structure to expand the lispd_mapping_elt used in lispd_map_cache_entry
 */
typedef struct rmt_mapping_extended_info_ {
    balancing_locators_vecs               rmt_balancing_locators_vecs;
}rmt_mapping_extended_info;


/****************************************  FUNCTIONS **************************************/

/*
 * Generates a basic mapping
 */
lispd_mapping_elt *new_mapping(
        lisp_addr_t     eid_prefix,
        uint8_t         eid_prefix_length,
        int             iid);

/*
 * Generates a mapping with the local extended info
 */

lispd_mapping_elt *new_local_mapping(
        lisp_addr_t     eid_prefix,
        uint8_t         eid_prefix_length,
        int             iid);

/*
 * Generates a mapping with the remote extended info
 */

lispd_mapping_elt *new_map_cache_mapping(
        lisp_addr_t     eid_prefix,
        uint8_t         eid_prefix_length,
        int             iid);

/**
 * Generates a clone of a lispd_mapping_elt. Parameters like timers or nonces are not cloned
 * @param elt lispd_mapping_elt to be cloned
 * @return cloned lispd_mapping_elt structure
 */
lispd_mapping_elt *copy_mapping_elt(lispd_mapping_elt *elt);

/*
 * Add a locator into the locators list of the mapping.
 */

int add_locator_to_mapping(
        lispd_mapping_elt           *mapping,
        lispd_locator_elt           *locator);

/*
 * Reinsert a locator into the locators list of the mapping.
 */
int reinsert_locator_to_mapping(
        lispd_mapping_elt           *mapping,
        lispd_locator_elt           *locator);

/*
 * Remove (free memory) the locator from the mapping with the specified address
 */
int remove_locator_from_mapping(
        lispd_mapping_elt       *mapping,
        lisp_addr_t             *loc_addr);

/*
 * This function sort the locator list elt with IP = changed_loc_addr
 */

void sort_locators_list_elt (
        lispd_mapping_elt   *mapping,
        lisp_addr_t         *changed_loc_addr);

/*
 * Returns the locators with the address passed as a parameter
 */

lispd_locator_elt *get_locator_from_mapping(
        lispd_mapping_elt   *mapping,
        lisp_addr_t         *address);

/*
 * Returns the locators with the nonce passed as a parameter
 */
lispd_locator_elt *get_locator_from_mapping_with_nonce(
        lispd_mapping_elt   *mapping,
        uint64_t            nonce,
        uint8_t             msg_type);

/*
 * Free memory of lispd_mapping_elt.
 */
void free_mapping_elt(lispd_mapping_elt *mapping);

/*
 * dump mapping
 */
void dump_mapping_entry(
        lispd_mapping_elt       *mapping,
        int                     log_level);

/*
 * Calculate the vectors used to distribute the load from the priority and weight of the locators of the mapping
 */
int calculate_balancing_vectors (
        lispd_mapping_elt           *mapping,
        balancing_locators_vecs     *b_locators_vecs);

/*
 * Print balancing locators vector information
 */

void dump_balancing_locators_vec(
        balancing_locators_vecs b_locators_vecs,
        lispd_mapping_elt *mapping,
        int log_level);


/**
 * Add a mapping to a mapping list
 * @param mapping Mapping element to be added
 * @param list Pointer to the first element of the list where to add the mapping
 * @retun GOOD if finish correctly or an error code otherwise
 */

int add_mapping_to_list(lispd_mapping_elt *mapping, lispd_mapping_list **list);

/*
 * Remove a mapping from a mapping list
 * @param mapping Mapping to be removed
 * @param list Pointer to the first element of the list where to remove the mapping list elt
 */
void remove_mapping_from_list(
        lispd_mapping_elt    *mapping,
        lispd_mapping_list   **list);

/**
 * Check if a mapping is already in the list
 * @param mapping Mapping element to be found
 * @param list List where to find the mapping
 * @retun TRUE if the mapping belongs to the list
 */
uint8_t is_mapping_in_the_list(
        lispd_mapping_elt   *mapping,
        lispd_mapping_list  *list);

/**
 * Retun the number of mappings of the list
 * @param list Mapping element to be added
 * @param list Pointer to the first element of the list where to add the mapping
 * @retun Number of mappings of the list
 */
int get_mapping_list_length(lispd_mapping_list   *list);

/**
 * Release the memory of a list of mappings
 * @param list First element of the list to be released
 * @param free_mappings If TRUE the elements stored in the list are also released
 */
void free_mapping_list(lispd_mapping_list *list, uint8_t free_mappings);

/*
 * Return the list of unique RTRs of the mapping
 */
lispd_rtr_locators_list *get_rtr_list_from_mapping(lispd_mapping_elt *mapping);


#endif /* LISPD_MAPPING_H_ */
